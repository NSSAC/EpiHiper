// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
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

#include <cstring>
#include <jansson.h>

#include "db/CTable.h"
#include "db/CField.h"
#include "utilities/CLogger.h"

CTable::CTable()
  : mId()
  , mLabel()
  , mFields()
  , mValid(false)
{}

CTable::CTable(const CTable & src)
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
  mValid = false; // DONE
  json_t * pValue = json_object_get(json, "name");

  if (!json_is_string(pValue))
    {
      CLogger::error("Table: Invalid or missing 'name'.");
      return;
    }

  mId = json_string_value(pValue);

  if (mId.empty())
    {
      CLogger::error("Table: Invalid empty 'name'.");
      return;
    }

  mLabel = mId;

  pValue = json_object_get(json, "title");

  if (json_is_string(pValue))
    {
      mLabel = json_string_value(pValue);
    }

  json_t * pSchema = json_object_get(json, "schema");

  if (!json_is_object(pSchema))
    {
      CLogger::error("Table: Invalid or missing 'schema'.");
      return;
    }

  pValue = json_object_get(pSchema, "fields");

  if (!json_is_array(pValue))
    {
      CLogger::error("Table: Invalid or missing 'fields'.");
      return;
    }

  for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i)
    {
      CField Field;
      Field.fromJSON(json_array_get(pValue, i));

      if (Field.isValid())
        {
          mFields.insert(std::make_pair(Field.getId(), Field));
        }
      else
        {
          CLogger::error() << "Table: Invalid field for item '" << i << "'.";
          return;
        }
    }

  pValue = json_object_get(pSchema, "primaryKey");

  if (json_is_string(pValue))
    {
      if (strcmp(json_string_value(pValue), "pid") != 0)
        {
          CLogger::error("Table: Invalid value for 'primaryKey'.");
          return;
        }
    }
  else if (json_is_array(pValue)
           && json_array_size(pValue) == 1)
    {
      json_t * pKey = json_array_get(pValue, 0);

      if (strcmp(json_string_value(pKey), "pid") != 0)
        {
          CLogger::error("Table: Invalid value for 'primaryKey'.");
          return;
        }
    }
  else
    {
      CLogger::error("Table: Missing value for 'primaryKey'.");
      return;
    }

  mValid = true;
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
