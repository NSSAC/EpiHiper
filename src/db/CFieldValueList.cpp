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

#include "db/CFieldValueList.h"
#include "db/CFieldValue.h"

CFieldValueList::CFieldValueList(const Type & type)
  : CValueList(type)
{}

CFieldValueList::CFieldValueList(const CFieldValueList & src)
  : CValueList(src)
{}

CFieldValueList::CFieldValueList(const json_t * json)
  : CValueList()
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
  json_t * pArray = json_object_get(json, "valueList");

  if (!json_is_array(pArray) ||
      json_array_size(pArray) == 0)
    {
      mValid = false;
      return;
    }

  json_t * pValue = json_array_get(pArray, 0);

  if (json_is_real(pValue))
    {
      mType = Type::number;
    }
  else if (json_is_string(pValue))
    {
      mType = Type::string;
    }
  else
    {
      mValid = false;
      return;
    }

  switch (mType)
  {
    case Type::number:
      for (size_t i = 0, imax = json_array_size(pArray); i < imax && mValid; ++i)
        {
          pValue = json_array_get(pArray, i);

          if (json_is_real(pValue))
            std::set< CValue >::insert(CFieldValue(json_real_value(pValue)));
          else
            mValid = false;
        }
      break;

    case Type::string:
      for (size_t i = 0, imax = json_array_size(pArray); i < imax && mValid; ++i)
        {
          pValue = json_array_get(pArray, i);

          if (json_is_string(pValue))
            std::set< CValue >::insert(CFieldValue(json_string_value(pValue)));
          else
            mValid = false;
        }
      break;
  }
}



