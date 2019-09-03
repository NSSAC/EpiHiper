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
#include <mpp/shmem.h>

#include "variables/CVariable.h"

CVariable::CVariable(const CVariable & src)
  : CAnnotation(src)
  , mId(src.mId)
  , mType(src.mType)
  , mInitialValue(src.mInitialValue)
  , mpValue(src.mpValue)
  , mResetValue(src.mResetValue)
  , mValid(src.mValid)
{}

CVariable::CVariable(const json_t * json)
  : CAnnotation()
  , mId()
  , mType()
  , mInitialValue()
  , mpValue(NULL)
  , mResetValue(std::numeric_limits< double >::quiet_NaN())
  , mValid(true)
{
  fromJSON(json);
}

// virtual
CVariable::~CVariable()
{
  if (mpValue != NULL)
    {
      // TODO this is a memory leak but we cannot delete it unless we do use reference counting;
      // shfree(mpValue);
    }
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

  mpValue = (double *) shmalloc(sizeof(double));
  reset();
  shmem_barrier_all();

  pValue = json_object_get(json, "reset");

  if (json_is_real(pValue))
    {
      mResetValue = json_real_value(pValue);
    }

  CAnnotation::fromJSON(json);
}

// virtual
void CVariable::compute()
{
  // TODO CRITICAL Implement me!
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

void CVariable::reset()
{
  *mpValue = mInitialValue;
}

bool CVariable::setValue(double value, CValueInterface::pOperator pOperator, const CMetadata & metadata)
{
  if (mType == Type::local ||
      CCommunicate::SHMEMRank == 0)
    {
      (*pOperator)(*mpValue, value);
    }
  else
    {
      long Lock;
      shmem_set_lock(&Lock);

      *mpValue = shmem_double_g(mpValue, 0);
      (*pOperator)(*mpValue, value);
      shmem_double_p(mpValue, *mpValue, 0);

      shmem_clear_lock(&Lock);
      shmem_fence();
    }

  return true;
}


