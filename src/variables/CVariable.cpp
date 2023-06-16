// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2023 Rector and Visitors of the University of Virginia 
//  
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal 
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions: 
//  
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software. 
//  
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE 
// END: Copyright 

#include <iostream>
#include <limits>
#include <cmath>
#include <jansson.h>

#include "utilities/CCommunicate.h"
#include "variables/CVariable.h"
#include "variables/CVariableList.h"
#include "actions/CActionQueue.h"
#include "utilities/CMetadata.h"
#include "utilities/CLogger.h"
#include "math/CValue.h"

// static 
CVariable * CVariable::transmissibility()
{
  CVariable * pTransmissibility = new CVariable();

  pTransmissibility->mId = "%transmissibility%";
  pTransmissibility->mScope = Scope::local;
  pTransmissibility->mInitialValue = 1.0;
  pTransmissibility->CComputable::mValid = true;

  return pTransmissibility;
}

CVariable::CVariable()
  : CValueInterface(CValueInterface::Type::number, NULL)
  , CComputable()
  , CAnnotation()
  , mId()
  , mScope()
  , mInitialValue(std::numeric_limits< double >::quiet_NaN())
  , mLocalValue()
  , mResetValue(0)
  , mIndex(std::numeric_limits< size_t >::max())
{
  mLocalValue.init();
}


CVariable::CVariable(const CVariable & src)
  : CValueInterface(src)
  , CComputable(src)
  , CAnnotation(src)
  , mId(src.mId)
  , mScope(src.mScope)
  , mInitialValue(src.mInitialValue)
  , mLocalValue(src.mLocalValue)
  , mResetValue(src.mResetValue)
  , mIndex(src.mIndex)
{}

CVariable::CVariable(const json_t * json)
  : CValueInterface(CValueInterface::Type::number, NULL)
  , CComputable()
  , CAnnotation()
  , mId()
  , mScope()
  , mInitialValue(std::numeric_limits< double >::quiet_NaN())
  , mLocalValue()
  , mResetValue(0)
  , mIndex(std::numeric_limits< size_t >::max())
{
  mLocalValue.init();

  fromJSON(json);

  if (isValid()
      && mScope == Scope::global)
    {
      mIndex = CCommunicate::getRMAIndex();
    }
}

// virtual
CVariable::~CVariable()
{}

void CVariable::fromJSON(const json_t * json)
{
  /*
    "variable": {
      "$id": "#variable",
      "description": "A variable",
      "allOf": [
        {"$ref": "#/definitions/annotation"},
        {
          "type": "object",
          "required": [
            "id",
            "initialValue",
            "scope"
          ],
          "properties": {
            "id": {"$ref": "#/definitions/uniqueId"},
            "initialValue": {"$ref": "#/definitions/nonNegativeNumber"},
            "scope": {"$ref": "#/definitions/scope"},
            "reset": {"$ref": "#/definitions/nonNegativeInteger"}
          },
          "patternProperties": {
            "^ann:": {}
          },
          "additionalProperties": false
        }
      ]
    },
  */

  if (json == NULL)
    return;
    
  CComputable::mValid = false; // DONE
  json_t * pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
      mId = json_string_value(pValue);
      mAnnId = mId;
    }

  if (mId.empty())
    {
      CLogger::error("Variable: Invalid or missing value for 'id'.");
      return;
    }

  CAnnotation::fromJSON(json);
  pValue = json_object_get(json, "initialValue");

  if (json_is_real(pValue))
    {
      mInitialValue = json_real_value(pValue);

      mLocalValue.Master() = mInitialValue;

      double * pIt = mLocalValue.beginThread();
      double * pEnd = mLocalValue.endThread();

      for (; pIt != pEnd; ++pIt)
        *pIt = mInitialValue;
    }
  else
    {
      CLogger::error("Variable: Invalid or missing value for 'initialValue'.");
      return;
    }

  pValue = json_object_get(json, "scope");

  if (json_is_string(pValue))
    {
      std::string(json_string_value(pValue));
      mScope = std::string(json_string_value(pValue)) == "global" ? Scope::global : Scope::local;
    }
  else
    {
      CLogger::error("Variable: Invalid or missing value for 'scope'.");
      return;
    }

  pValue = json_object_get(json, "reset");

  if (json_is_real(pValue))
    {
      mResetValue = json_real_value(pValue);
    }
  else
    {
      mResetValue = 0;
    }

  CComputable::mValid = true;
}

void CVariable::toBinary(std::ostream & os) const
{
  os.write(reinterpret_cast< const char * >(&mInitialValue), sizeof(double));
}

void CVariable::fromBinary(std::istream & is)
{
  is.read(reinterpret_cast< char * >(&mInitialValue), sizeof(double));
}

const std::string & CVariable::getId() const
{
  return mId;
}

bool CVariable::isValid() const
{
  return CComputable::mValid;
}

const CVariable::Scope & CVariable::getScope() const
{
  return mScope;
}

// virtual 
void CVariable::determineIsStatic()
{
  mStatic = false;
}

bool CVariable::reset(const bool & force)
{
  // We are guaranteed to be called only by one thread per family.
  bool changed = false;

  if ((mResetValue != 0
       && CActionQueue::getCurrentTick() % mResetValue == 0)
      || force)
    {
      changed = (mLocalValue.Master() != mInitialValue) || force;
      mLocalValue.Master() = mInitialValue;

      if (CCommunicate::MPIRank == 0
          && mScope == Scope::global
          && CCommunicate::MPIProcesses > 1)
        {
          CCommunicate::updateRMA(mIndex, &CValueInterface::equal, mLocalValue.Master());
        }

      double * pIt = mLocalValue.beginThread();
      double * pEnd = mLocalValue.endThread();
      
      if (mLocalValue.isThread(pIt))
        for (; pIt != pEnd; ++pIt)
          *pIt = mLocalValue.Master();
    }

  return changed;
}

bool CVariable::getValue()
{
  bool changed = false;

  if (mScope == Scope::global &&
      CCommunicate::MPIProcesses > 1)
    {
      double & Value = mLocalValue.Active();
      const double OldValue = Value;

      Value = CCommunicate::getRMA(mIndex);

      changed = (Value != OldValue);
    }

  return changed;
}

bool CVariable::setValue(const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & ENABLE_TRACE(metadata))
{
  const double & OperatorValue = value.toNumber();
  ENABLE_TRACE(CLogger::trace("CVariable [ActionDefinition: {}]: Variable ({}) value {} {}",
                              metadata.contains("CActionDefinition") ? metadata.getInt("CActionDefinition") : -1,
                              mId,
                              CValueInterface::operatorToString(pOperator),
                              OperatorValue););

  double & Value = mLocalValue.Active();
  const double OldValue = Value;

  if (mScope == Scope::global
      && CCommunicate::MPIProcesses > 1)
    Value = CCommunicate::updateRMA(mIndex, pOperator, OperatorValue);
  else
    (*pOperator)(Value, OperatorValue);

  bool changed = (Value != OldValue);

  if (changed)
    CVariableList::INSTANCE.changedVariables().Active().insert(this);

  return changed;
}

void CVariable::updateMaster()
{
  mLocalValue.Master() = mLocalValue.Active();
}

void CVariable::setInitialValue(const double & initialValue)
{
  mInitialValue = initialValue;
}

CValueInterface CVariable::toValue() 
{
  getValue();
  return CValueInterface(mLocalValue.Active());
}

// virtual 
std::string CVariable::getComputableId() const
{
  std::ostringstream os;
  os << "CVariable: " << mId << " (" << mComputableId  << ")";

  return os.str();
}

// virtual 
bool CVariable::computeProtected()
{
  return true;
}
