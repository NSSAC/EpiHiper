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

#include <string>
#include <jansson.h>

#include "diseaseModel/CFactorOperation.h"

CFactorOperation::CFactorOperation()
  : mType(Type::__NONE)
  , mValue()
  , mValid(false)
{}

CFactorOperation::CFactorOperation(const CFactorOperation & src)
  : mType(src.mType)
  , mValue(src.mValue)
  , mValid(src.mValid)
{}

// virtual
CFactorOperation::~CFactorOperation()
{}

void CFactorOperation::fromJSON(const json_t * json)
{
  mValid = true;

  json_t * pValue = json_object_get(json, "operator");

  if (json_is_string(pValue))
    {
      std::string Operator = json_string_value(pValue);

      if (Operator == "=") mType = Type::assign;
      else if(Operator == "*=") mType = Type::multiply;
      else if (Operator == "/=") mType = Type::divide;
    }

  mValid &= (mType != Type::__NONE);

  pValue = json_object_get(json, "value");

  if (json_is_real(pValue))
    {
      mValue = json_real_value(pValue);
    }

  mValid &= (mValue >= 0);
}

const bool & CFactorOperation::isValid() const
{
  return mValid;
}

void CFactorOperation::apply(double & value) const
{
  switch (mType)
  {
    case Type::assign:
      value = mValue;
      break;

    case Type::multiply:
      value *= mValue;
      break;

    case Type::divide:
      value /= mValue;
      break;

    case Type::__NONE:
      break;
  }
}


