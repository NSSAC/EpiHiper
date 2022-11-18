// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
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

#include "db/CFieldValueList.h"
#include "db/CFieldValue.h"
#include "utilities/CLogger.h"

CFieldValueList::CFieldValueList(const Type & type)
  : CValueList(type)
{}

CFieldValueList::CFieldValueList(const CFieldValueList & src)
  : CValueList(src)
{}

CFieldValueList::CFieldValueList(const json_t * json, const Type & hint)
  : CValueList(hint)
{
  fromJSON(json);
}

// virtual
CFieldValueList::~CFieldValueList()
{}

bool CFieldValueList::append(const CFieldValue & value)
{
  if (mType == Type::boolean)
    mType = value.getType();

  return CValueList::append(value);
}

// virtual
void CFieldValueList::fromJSON(const json_t * json)
{
  mValid = false; // DONE
  json_t * pArray = json_object_get(json, "valueList");

  // We only report an error if json contains a 'valueList'
  if (pArray == NULL)
    {
      return;
    }

  if (!json_is_array(pArray)
      || json_array_size(pArray) == 0)
    {
      CLogger::error("Field value list: Invalid or missing 'valueList'.");
      return;
    }

  json_t * pValue = json_array_get(pArray, 0);

  mValid = true;

  switch (mType)
    {
    case Type::id:
      for (size_t i = 0, imax = json_array_size(pArray); i < imax && mValid; ++i)
        {
          pValue = json_array_get(pArray, i);

          if (json_is_real(pValue) &&
              abs(json_real_value(pValue) - (size_t) json_real_value(pValue)) <= abs(json_real_value(pValue)) * 100.0 * std::numeric_limits< double >::epsilon())
            std::set< CValue >::insert(CFieldValue((size_t) json_real_value(pValue)));
          else
            {
              CLogger::error() << "Field value list: Invalid type for index '" << i << "'.";
              mValid = false; // DONE
            }
        }
      break;

    case Type::integer:
      for (size_t i = 0, imax = json_array_size(pArray); i < imax && mValid; ++i)
        {
          pValue = json_array_get(pArray, i);

          if (json_is_real(pValue) &&
              abs(json_real_value(pValue) - (int) json_real_value(pValue)) <= abs(json_real_value(pValue)) * 100.0 * std::numeric_limits< double >::epsilon())
            std::set< CValue >::insert(CFieldValue((int) json_real_value(pValue)));
          else
            {
              CLogger::error() << "Field value list: Invalid type for index '" << i << "'.";
              mValid = false; // DONE
            }
        }
      break;

    case Type::number:
      for (size_t i = 0, imax = json_array_size(pArray); i < imax && mValid; ++i)
        {
          pValue = json_array_get(pArray, i);

          if (json_is_real(pValue))
            std::set< CValue >::insert(CFieldValue(json_real_value(pValue)));
          else
            {
              CLogger::error() << "Field value list: Invalid type for index '" << i << "'.";
              mValid = false; // DONE
            }
        }
      break;

    case Type::string:
      for (size_t i = 0, imax = json_array_size(pArray); i < imax && mValid; ++i)
        {
          pValue = json_array_get(pArray, i);

          if (json_is_string(pValue))
            std::set< CValue >::insert(CFieldValue(json_string_value(pValue)));
          else
            {
              CLogger::error() << "Field value list: Invalid type for index '" << i << "'.";
              mValid = false; // DONE
            }
        }
      break;

      case Type::boolean:
      case Type::traitData:
      case Type::traitValue:
        break;
    }
}
