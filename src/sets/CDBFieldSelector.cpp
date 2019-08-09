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

#include "sets/CDBFieldSelector.h"
#include "db/CSchema.h"

CDBFieldSelector::CDBFieldSelector()
  : CSetContent()
  , mTable()
  , mField()
  , mFieldType()
  , mpSetContent(NULL)
{}

CDBFieldSelector::CDBFieldSelector(const CDBFieldSelector & src)
  : CSetContent(src)
  , mTable()
  , mField()
  , mFieldType(src.mFieldType)
  , mpSetContent(CSetContent::copy(src.mpSetContent))
{}

CDBFieldSelector::CDBFieldSelector(const json_t * json)
  : CSetContent()
  , mTable()
  , mField()
  , mFieldType()
  , mpSetContent(NULL)
{
  fromJSON(json);
}

CDBFieldSelector::~CDBFieldSelector()
{
  CSetContent::destroy(mpSetContent);
}

// virtual
void CDBFieldSelector::fromJSON(const json_t * json)
{
  json_t * pValue = json_object_get(json, "elementType");

  mValid = (json_is_string(pValue) && strcmp(json_string_value(pValue), "dbField") == 0);

  if (!mValid) return;

  pValue = json_object_get(json, "table");

  if (json_is_string(pValue))
    {
      mTable = json_string_value(pValue);
      mValid &= CSchema::INSTANCE.getTable(mTable).isValid();
    }
  else
    {
      mValid = false;
    }

  pValue = json_object_get(json, "field");

  if (json_is_string(pValue))
    {
      mField = json_string_value(pValue);

      const CField & Field = CSchema::INSTANCE.getTable(mTable).getField(mField);
      mFieldType = Field.getType();
      mValid &= Field.isValid();
    }
  else
    {
      mValid = false;
    }

  pValue = json_object_get(json, "selector");

  if (json_is_object(pValue))
    {
      mpSetContent = CSetContent::create(pValue);
      mValid &= (mpSetContent != NULL && mpSetContent->isValid());
    }
  else
    {
      mValid = false;
    }
}


// virtual
void CDBFieldSelector::compute()
{
  // TODO CRITICAL Implement me!
}


