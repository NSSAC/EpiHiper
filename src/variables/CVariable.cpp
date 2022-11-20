// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
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
#include "actions/CActionQueue.h"
#include "utilities/CMetadata.h"
#include "utilities/CLogger.h"

// static 
CVariable * CVariable::transmissibility()
{
  CVariable * pTransmissibility = new CVariable();

  pTransmissibility->mId = "%transmissibility%";
  pTransmissibility->mType = Type::local;
  pTransmissibility->mInitialValue = 1.0;
  pTransmissibility->mValid = true;

  return pTransmissibility;
}

CVariable::CVariable()
  : CValue(std::numeric_limits< double >::quiet_NaN())
  , CAnnotation()
  , mId()
  , mType()
  , mInitialValue(std::numeric_limits< double >::quiet_NaN())
  , mpLocalValue(static_cast< double * >(mpValue))
  , mResetValue(0)
  , mIndex(-1)
  , mValid(false)
{}

CVariable::CVariable(const CVariable & src)
  : CValue(src)
  , CAnnotation(src)
  , mId(src.mId)
  , mType(src.mType)
  , mInitialValue(src.mInitialValue)
  , mpLocalValue(static_cast< double * >(mpValue))
  , mResetValue(src.mResetValue)
  , mIndex(src.mIndex)
  , mValid(src.mValid)
{}

CVariable::CVariable(const json_t * json)
  : CValue(std::numeric_limits< double >::quiet_NaN())
  , CAnnotation()
  , mId()
  , mType()
  , mInitialValue(std::numeric_limits< double >::quiet_NaN())
  , mpLocalValue(static_cast< double * >(mpValue))
  , mResetValue(0)
  , mIndex(-1)
  , mValid(true)
{
  fromJSON(json);

  if (mValid
      && mType == Type::global)
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
    
  mValid = false; // DONE
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
      mType = std::string(json_string_value(pValue)) == "global" ? Type::global : Type::local;
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

  mValid = true;
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

const bool & CVariable::isValid() const
{
  return mValid;
}

const CVariable::Type & CVariable::getType() const
{
  return mType;
}

void CVariable::reset(const bool & force)
{
  if ((mResetValue != 0
       && CActionQueue::getCurrentTick() % mResetValue == 0)
      || force)
    {
#pragma omp atomic write
      *mpLocalValue = mInitialValue;

      if (CCommunicate::MPIRank == 0
          && mType == Type::global
          && CCommunicate::MPIProcesses > 1)
        {
          CCommunicate::updateRMA(mIndex, &CValueInterface::equal, *mpLocalValue);
        }
    }
}

void CVariable::getValue()
{
  if (mType == Type::global &&
      CCommunicate::MPIProcesses > 1)
    {
      double Value = CCommunicate::getRMA(mIndex);

#pragma omp atomic write
      *mpLocalValue = Value;
    }
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

  if (mType == Type::global
      && CCommunicate::MPIProcesses > 1)
    Value = CCommunicate::updateRMA(mIndex, pOperator, value);
  else
    (*pOperator)(Value, value);

#pragma omp atomic write
  *mpLocalValue = Value;

  return true;
}

bool CVariable::setValue(const CValue value, CValueInterface::pOperator pOperator, const CMetadata & metadata)
{
  return setValue(value.toNumber(), pOperator, metadata);
}

void CVariable::setInitialValue(const double & initialValue)
{
  mInitialValue = initialValue;
}

