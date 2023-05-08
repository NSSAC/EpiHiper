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
  : CValue(std::numeric_limits< double >::quiet_NaN())
  , CComputable()
  , CAnnotation()
  , mId()
  , mScope()
  , mInitialValue(std::numeric_limits< double >::quiet_NaN())
  , mpLocalValue(static_cast< double * >(mpValue))
  , mResetValue(0)
  , mIndex(-1)
{}

CVariable::CVariable(const CVariable & src)
  : CValue(src)
  , CComputable(src)
  , CAnnotation(src)
  , mId(src.mId)
  , mScope(src.mScope)
  , mInitialValue(src.mInitialValue)
  , mpLocalValue(static_cast< double * >(mpValue))
  , mResetValue(src.mResetValue)
  , mIndex(src.mIndex)
{}

CVariable::CVariable(const json_t * json)
  : CValue(std::numeric_limits< double >::quiet_NaN())
  , CComputable()
  , CAnnotation()
  , mId()
  , mScope()
  , mInitialValue(std::numeric_limits< double >::quiet_NaN())
  , mpLocalValue(static_cast< double * >(mpValue))
  , mResetValue(0)
  , mIndex(-1)
{
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

// virtual
CValueInterface * CVariable::copy() const
{
  return new CVariable(*this);
}

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
      *mpLocalValue = mInitialValue;
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
  return CValue::mValid && CComputable::mValid;
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
  bool changed = false;

  if ((mResetValue != 0
       && CActionQueue::getCurrentTick() % mResetValue == 0)
      || force)
    {
      changed = (*mpLocalValue != mInitialValue) || force;

      *mpLocalValue = mInitialValue;

      if (CCommunicate::MPIRank == 0
          && mScope == Scope::global
          && CCommunicate::MPIProcesses > 1)
        {
          CCommunicate::updateRMA(mIndex, &CValueInterface::equal, *mpLocalValue);
        }
    }

  return changed;
}

bool CVariable::getValue()
{
  bool changed = false;

  if (mScope == Scope::global &&
      CCommunicate::MPIProcesses > 1)
    {
      double Value = CCommunicate::getRMA(mIndex);

      changed = (*mpLocalValue != Value);

#pragma omp atomic write
      *mpLocalValue = Value;
    }

  return changed;
}

bool CVariable::setValue(double value, CValueInterface::pOperator pOperator, const CMetadata & metadata)
{
  CLogger::trace() << "CVariable [ActionDefinition:"
                   << (metadata.contains("CActionDefinition") ? metadata.getInt("CActionDefinition") : -1)
                   << "]: Variable ("
                   << mId
                   << ") value "
                   << CValueInterface::operatorToString(pOperator)
                   << " "
                   << value;

  double Value = *mpLocalValue;

  if (mScope == Scope::global
      && CCommunicate::MPIProcesses > 1)
    Value = CCommunicate::updateRMA(mIndex, pOperator, value);
  else
    (*pOperator)(Value, value);

  bool changed = (*mpLocalValue != Value);

#pragma omp atomic write
  *mpLocalValue = Value;

  if (changed)
    {
      if (mScope == Scope::global
          && CCommunicate::MPIProcesses > 1)
        CVariableList::INSTANCE.changedVariables().Active().insert(this);
      else
        CVariableList::INSTANCE.changedVariables().Master().insert(this);
    }

  return changed;
}

bool CVariable::setValue(const CValue value, CValueInterface::pOperator pOperator, const CMetadata & metadata)
{
  return setValue(value.toNumber(), pOperator, metadata);
}

void CVariable::setInitialValue(const double & initialValue)
{
  mInitialValue = initialValue;
}

// virtual 
std::string CVariable::getComputableId() const
{
  return "CVariable:" + mId;
}

// virtual 
bool CVariable::computeProtected()
{
  return true;
}
