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

#include <string>
#include <jansson.h>

#include "diseaseModel/CFactorOperation.h"
#include "utilities/CLogger.h"
#include "utilities/CSimConfig.h"

CFactorOperation::CFactorOperation()
  : mType(Type::__NONE)
  , mValue(0.0)
  , mValid(false)
  , mNormalizedJSON("{}")
{}

CFactorOperation::CFactorOperation(const CFactorOperation & src)
  : mType(src.mType)
  , mValue(src.mValue)
  , mValid(src.mValid)
  , mNormalizedJSON(src.mNormalizedJSON)
{}

// virtual
CFactorOperation::~CFactorOperation()
{}

void CFactorOperation::fromJSON(const json_t * json)
{
  mValid = false;
  mNormalizedJSON = "{}";
  mType = Type::__NONE;


  if (json_object_size(json) == 0)
    {
      mValid = true;
      return;
    }
    
  json_t * pValue = json_object_get(json, "operator");

  if (json_is_string(pValue))
    {
      std::string Operator = json_string_value(pValue);

      if (Operator == "=")
        mType = Type::assign;
      else if (Operator == "*=")
        mType = Type::multiply;
      else if (Operator == "/=")
        mType = Type::divide;
    }

  if (mType == Type::__NONE)
    {
      CLogger::error("Operation: Invalid or missing 'operator'.");
      return;
    }

  pValue = json_object_get(json, "value");
  mValue = -1.0;

  if (json_is_real(pValue))
    {
      mValue = json_real_value(pValue);
    }

  if (mValue < 0.0)
    {
      CLogger::error("Operation: Invalid or missing 'value'.");
      return;
    }

  mValid = true;
  normalizeJSON();
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

void CFactorOperation::normalizeJSON()
{
  if (mType == Type::__NONE)
    mNormalizedJSON = "{}";

  json_error_t error;
  json_t * pJson = json_loads("{}", JSON_DECODE_INT_AS_REAL, &error);

  switch (mType)
    {
    case Type::assign:
      json_object_set_new(pJson, "operator", json_string("="));
      break;

    case Type::multiply:
      json_object_set_new(pJson, "operator", json_string("*="));
      break;

    case Type::divide:
      json_object_set_new(pJson, "operator", json_string("/="));
      break;

    case Type::__NONE:
      break;
    }

  json_object_set_new(pJson, "value", json_real(mValue));
  mNormalizedJSON = CSimConfig::jsonToString(pJson);
  json_decref(pJson);
}

std::string & CFactorOperation::getJson()
{
  return mNormalizedJSON;
}

bool CFactorOperation::setJson(const std::string & json)
{
  if (json == mNormalizedJSON)
    return false;

  json_error_t error;
  json_t * pJson = json_loads(json.c_str(), JSON_DECODE_INT_AS_REAL, &error);
  fromJSON(pJson);
  json_decref(pJson);

  return true;
}

