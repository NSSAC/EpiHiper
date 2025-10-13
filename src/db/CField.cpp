// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2025 Rector and Visitors of the University of Virginia 
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

#include "db/CField.h"
#include "db/CConnection.h"
#include "db/CSchema.h"
#include "db/CQuery.h"
#include "db/CFieldValue.h"
#include "utilities/CLogger.h"

CField::CField()
  : mId()
  , mLabel()
  , mType()
  , mValidValues()
  , mValid(false)
  , mDBType()
{}

CField::CField(const CField & src)
  : mId(src.mId)
  , mLabel(src.mLabel)
  , mType(src.mType)
  , mValidValues(src.mValidValues)
  , mValid(src.mValid)
  , mDBType(src.mDBType)
{}

// virtual
CField::~CField()
{}

void CField::fromJSON(const json_t * json)
{
  mValid = false; // DONE
  mValidValues.clear();
  mDBType.clear();

  json_t * pValue = json_object_get(json, "name");

  if (!json_is_string(pValue))
    {
      CLogger::error("Field: Invalid or missing 'name'.");
      return;
    }

  mId = json_string_value(pValue);

  if (mId.empty())
    {
      CLogger::error("Field: Invalid empty 'name'.");
      return;
    }

  mLabel = mId;

  if (mId == "pid"
      || mId == "hid"
      || mId == "lid")
    {
      mType = CValue::Type::id;
    }
  else
    {
      pValue = json_object_get(json, "type");

      if (json_is_string(pValue))
        {
          if (strcmp(json_string_value(pValue), "string") == 0)
            {
              mType = CValue::Type::string;
            }
          else if (strcmp(json_string_value(pValue), "number") == 0)
            {
              mType = CValue::Type::number;
            }
          else if (strcmp(json_string_value(pValue), "integer") == 0)
            {
              mType = CValue::Type::integer;
            }
          else
            {
              CLogger::error("Field: Invalid or missing 'type'.");
              return;
            }
        }
    }

  if (mType == CValue::Type::string)
    {
      pValue = json_object_get(json, "constraints");

      if (json_is_object(pValue))
        {
          json_t * pEnums = json_object_get(pValue, "enum");

          if (json_is_array(pEnums))
            for (size_t i = 0, imax = json_array_size(pEnums); i < imax; ++i)
              mValidValues.insert(json_string_value(json_array_get(pEnums, i)));
        }
    }
  
  mValid = true;
}

const std::string & CField::getId() const
{
  return mId;
}

const bool & CField::isValid() const
{
  return mValid;
}

const CValue::Type & CField::getType() const
{
  return mType;
}

bool CField::isValidValue(const std::string & value) const
{
  if (mValidValues.empty())
    return true;
    
  return mValidValues.find(value) != mValidValues.end();
}

bool CField::hasConstraints() const
{
  return !mValidValues.empty();
}

const std::set< std::string > & CField::getConstraints() const
{
  return mValidValues;
}

const std::string & CField::getDBType() const
{
  if (mDBType.empty())
    {
      std::ostringstream Query;

      // TODO CRITICAL This postgresql specif and needs to be pushed the appropriate class
      Query << "SELECT ";
      Query << "    pg_catalog.format_type(f.atttypid,f.atttypmod) AS type ";
      Query << "FROM ";
      Query << "    pg_attribute f ";
      Query << "    JOIN pg_class c ON c.oid = f.attrelid ";
      Query << "    JOIN pg_type t ON t.oid = f.atttypid ";
      Query << "    LEFT JOIN pg_attrdef d ON d.adrelid = c.oid AND d.adnum = f.attnum ";
      Query << "    LEFT JOIN pg_namespace n ON n.oid = c.relnamespace ";
      Query << "    LEFT JOIN pg_constraint p ON p.conrelid = c.oid AND f.attnum = ANY (p.conkey) ";
      Query << "    LEFT JOIN pg_class AS g ON p.confrelid = g.oid ";
      Query << "WHERE c.relkind = 'r'::char ";
      Query << "    AND n.nspname = 'public' ";
      Query << "    AND c.relname = '" << CSchema::INSTANCE.getTableForField(mId) << "' "; 
      Query << "    AND f.attname = '" << mId << "' ";
      Query << "    AND f.attnum > 0"; 
      
      for (const pqxx::row & Row : CQuery::sql(Query.str()))
        for (const pqxx::field & Field : Row)
          {
            mDBType = CFieldValue(Field.as(std::string())).toString();

            if (!mDBType.empty())
              break;
          }
    }

  return mDBType;
}

