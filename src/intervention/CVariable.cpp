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

#include "CVariable.h"

CVariable::CVariable(const CVariable & src)
  : CAnnotation(src)
  , mId(src.mId)
  , mType(src.mType)
  , mValue(src.mValue)
  , mResetValue(src.mResetValue)
  , mValid(src.mValid)
{}

CVariable::CVariable(const json_t * json)
  : CAnnotation()
  , mId()
  , mType()
  , mValue()
  , mResetValue(std::numeric_limits< double >::quiet_NaN())
  , mValid(true)
{
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
      mValue = std::round(json_real_value(pValue));
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
      mResetValue = json_real_value(pValue);
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

  CAnnotation::fromJSON(json);
}

// virtual
CVariable::~CVariable()
{}

void CVariable::fromJSON(const json_t * json)
{
  *this = CVariable(json);
}

void CVariable::toBinary(std::ostream & os) const
{
  os.write(reinterpret_cast<const char *>(&mValue), sizeof(double));
}

void CVariable::fromBinary(std::istream & is)
{
  is.read(reinterpret_cast<char *>(&mValue), sizeof(double));
}

const std::string & CVariable::getId() const
{
  return mId;
}

const bool & CVariable::isValid() const
{
  return mValid;
}


