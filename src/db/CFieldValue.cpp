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

#include <jansson.h>

#include "db/CFieldValue.h"

CFieldValue::CFieldValue(const double & number)
  : CValue(number)
{}

CFieldValue::CFieldValue(const std::string & str)
  : CValue(str)
{}

CFieldValue::CFieldValue(const json_t * json)
  : CValue(false)
{
  fromJSON(json);
}

// virtual
CFieldValue::~CFieldValue()
{}

// virtual
void CFieldValue::fromJSON(const json_t * json)
{
  json_t * pValue = json_object_get(json, "value");

  if (json_is_real(pValue))
    {
      destroyValue();
      mType = Type::number;
      mpValue = new double(json_real_value(pValue));
      return;
    }

  if (json_is_string(pValue))
    {
      destroyValue();
      mType = Type::string;
      mpValue = new std::string(json_string_value(pValue));
      return;
    }

  mValid = false;
  return;
}
