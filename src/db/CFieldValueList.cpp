// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

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

  if (json_is_real(pValue)
      && mType != Type::integer)
    {
      mType = Type::number;
    }
  else if (json_is_string(pValue))
    {
      mType = Type::string;
    }
  else
    {
      CLogger::error("Field value list: Invalid type for 'valueList'.");
      return;
    }

  mValid = true;

  switch (mType)
    {
    case Type::integer:
      for (size_t i = 0, imax = json_array_size(pArray); i < imax && mValid; ++i)
        {
          pValue = json_array_get(pArray, i);

          if (json_is_integer(pValue))
            std::set< CValue >::insert(CFieldValue((int) json_integer_value(pValue)));
          else
            {
              CLogger::error() << "Field value list: Invalid type for value '" << i << "'.";
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
              CLogger::error() << "Field value list: Invalid type for value '" << i << "'.";
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
              CLogger::error() << "Field value list: Invalid type for value '" << i << "'.";
              mValid = false; // DONE
            }
        }
      break;

      case Type::boolean:
      case Type::traitData:
      case Type::traitValue:
      case Type::id:
        break;
    }
}
