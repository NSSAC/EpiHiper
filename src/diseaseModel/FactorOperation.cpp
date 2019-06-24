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

#include "FactorOperation.h"

FactorOperation::FactorOperation()
  : mType(Type::__NONE)
  , mValue()
  , mValid(false)
{}

FactorOperation::FactorOperation(const FactorOperation & src)
  : mType(src.mType)
  , mValue(src.mValue)
  , mValid(src.mValid)
{}

// virtual
FactorOperation::~FactorOperation()
{}

void FactorOperation::fromJSON(const json_t * json)
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

const bool & FactorOperation::isValid() const
{
  return mValid;
}
