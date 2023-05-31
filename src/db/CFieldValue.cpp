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

#include <jansson.h>

#include "db/CFieldValue.h"
#include "utilities/CLogger.h"

CFieldValue::CFieldValue(const size_t & id)
  : CValue(id)
{}

CFieldValue::CFieldValue(const double & number)
  : CValue(number)
{}

CFieldValue::CFieldValue(const int & integer)
  : CValue(integer)
{}

CFieldValue::CFieldValue(const std::string & str)
  : CValue(str)
{}

CFieldValue::CFieldValue(const json_t * json, const Type & hint)
  : CValue(false)
{
  destroyValue();
  mType = hint;
  fromJSON(json);
}

// virtual
CFieldValue::~CFieldValue()
{}

// virtual
void CFieldValue::fromJSON(const json_t * json)
{
  mValid = false; // DONE
  destroyValue();

  json_t * pValue = json_object_get(json, "value");

  if (pValue == NULL)
    {
      CLogger::error("Field value: Missing 'value'.");
    }

  switch (mType)
    {
    case Type::id:
      if (json_is_real(pValue) 
          && json_real_value(pValue) - (size_t) json_real_value(pValue) < json_real_value(pValue) * 100.0 * std::numeric_limits< double >::epsilon())
        {
          mpValue = new size_t((size_t) json_real_value(pValue));
          mValid = true;
        }
      else
        {
          CLogger::error("Field value: Invalid type for 'value'.");
        }
      break;

    case Type::integer:
      if (json_is_real(pValue) 
          && json_real_value(pValue) - (int) json_real_value(pValue) < json_real_value(pValue) * 100.0 * std::numeric_limits< double >::epsilon())
        {
          mpValue = new int((int) json_real_value(pValue));
          mValid = true;
        }
      else
        {
          CLogger::error("Field value: Invalid type for 'value'.");
        }
      break;

    case Type::number:
      if (json_is_real(pValue))
        {
          mpValue = new double(json_real_value(pValue));
          mValid = true;
        }
      else
        {
          CLogger::error("Field value: Invalid type for 'value'.");
        }
      break;

    case Type::string:
      if (json_is_string(pValue))
        {
          mpValue = new std::string(json_string_value(pValue));
          mValid = true;
        }
      else
        {
          CLogger::error("Field value: Invalid type for 'value'.");
        }
      break;

    case Type::boolean:
    case Type::traitData:
    case Type::traitValue:
    case CValueInterface::Type::__SIZE:
      CLogger::error("Field value: Invalid type for 'value'.");
      mValid = false; // DONE
      break;
    }

  return;
}
