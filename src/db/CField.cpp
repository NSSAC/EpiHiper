// BEGIN: Copyright 
// Copyright (C) 2019 - 2021 Rector and Visitors of the University of Virginia 
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

#include "db/CField.h"
#include "utilities/CLogger.h"

CField::CField()
  : mId()
  , mLabel()
  , mType()
  , mValidValues()
  , mValid(false)
{}

CField::CField(const CField & src)
  : mId(src.mId)
  , mLabel(src.mLabel)
  , mType(src.mType)
  , mValidValues(src.mValidValues)
  , mValid(src.mValid)
{}

// virtual
CField::~CField()
{}

void CField::fromJSON(const json_t * json)
{
  mValid = false; // DONE
  mValidValues.clear();

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

