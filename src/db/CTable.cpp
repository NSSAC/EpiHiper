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

#include <cstring>
#include <jansson.h>

#include "db/CTable.h"
#include "db/CField.h"

CTable::CTable()
  : mId()
  , mLabel()
  , mFields()
  , mValid(false)
{}

CTable::CTable(const CTable &src)
  : mId(src.mId)
  , mLabel(src.mLabel)
  , mFields(src.mFields)
  , mValid(src.mValid)
{}

// virtual
CTable::~CTable()
{}

void CTable::fromJSON(const json_t * json)
{
  json_t * pValue = json_object_get(json, "name");

  if (json_is_string(pValue))
    {
      mId = json_string_value(pValue);
    }

  mValid = !mId.empty();
  mLabel = mId;

  pValue = json_object_get(json, "title");

  if (json_is_string(pValue))
    {
      mLabel = json_string_value(pValue);
    }

  json_t * pSchema = json_object_get(json, "schema");

  if (!json_is_object(pSchema))
    {
      mValid = false;
      return;
    }

  pValue = json_object_get(pSchema, "fields");

  if (!json_is_array(pValue))
    {
      mValid = false;
      return;
    }

  for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i)
    {
      CField Field;
      Field.fromJSON(json_array_get(pValue, i));
      mValid &= Field.isValid();

      mFields.insert(std::make_pair(Field.getId(), Field));
    }

  pValue = json_object_get(pSchema, "primaryKey");

  if (json_is_string(pValue))
    {
      mValid &= strcmp(json_string_value(pValue), "pid") == 0;
    }
  else if (json_is_array(pValue) && json_array_size(pValue) == 1)
    {
      json_t * pKey = json_array_get(pValue, 0);
      mValid &= json_is_string(pKey) && strcmp(json_string_value(pKey), "pid") == 0;
    }
}

const std::string & CTable::getId() const
{
  return mId;
}

const bool & CTable::isValid() const
{
  return mValid;
}

const std::map< std::string, CField > & CTable::getFields() const
{
  return mFields;
}


const CField & CTable::getField(const std::string & field) const
{
  static CField Invalid;

  std::map< std::string, CField >::const_iterator found = mFields.find(field);

  if (found != mFields.end())
    return found->second;

  return Invalid;
}
