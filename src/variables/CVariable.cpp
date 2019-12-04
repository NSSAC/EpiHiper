// BEGIN: Copyright 
// Copyright (C) 2019 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include <iostream>
#include <limits>
#include <cmath>
#include <jansson.h>

#include "utilities/CCommunicate.h"
#include "variables/CVariable.h"
#include "actions/CActionQueue.h"

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

  if (mValid &&
      mType == Type::global)
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

  json_t * pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
      mId = json_string_value(pValue);
      mAnnId = mId;
    }

  mValid &= !mId.empty();

  pValue = json_object_get(json, "initialValue");

  if (json_is_real(pValue))
    {
      mInitialValue = json_real_value(pValue);
      *mpLocalValue = mInitialValue;
    }
  else
    {
      mValid = false;
    }

  pValue = json_object_get(json, "scope");

  if (json_is_string(pValue))
    {
      std::string(json_string_value(pValue));
      mType = std::string(json_string_value(pValue)) == "global" ? Type::global : Type::local;
    }
  else
    {
      mValid = false;
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

  CAnnotation::fromJSON(json);
}

// virtual
void CVariable::process()
{
  if (mType == Type::global)
    {
      *mpLocalValue = CCommunicate::getRMA(mIndex);
    }
}

void CVariable::toBinary(std::ostream & os) const
{
  os.write(reinterpret_cast<const char *>(&mInitialValue), sizeof(double));
}

void CVariable::fromBinary(std::istream & is)
{
  is.read(reinterpret_cast<char *>(&mInitialValue), sizeof(double));
}

const std::string & CVariable::getId() const
{
  return mId;
}

const bool & CVariable::isValid() const
{
  return mValid;
}

void CVariable::reset(const bool & force)
{
  if ((mResetValue != 0 &&
       CActionQueue::getCurrentTick() % mResetValue == 0) ||
      force)
    {
      *mpLocalValue = mInitialValue;

      if (CCommunicate::MPIRank == 0 &&
          mType == Type::global)
        {
          CCommunicate::updateRMA(mIndex, &CValueInterface::equal, *mpLocalValue);
        }
    }
}

bool CVariable::setValue(double value, CValueInterface::pOperator pOperator, const CMetadata & metadata)
{

  if (mType == Type::global)
    {
      *mpLocalValue = CCommunicate::updateRMA(mIndex, pOperator, value);
    }
  else
    {
      (*pOperator)(*mpLocalValue, value);
    }

  return true;
}


