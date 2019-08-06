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

CDBFieldSelector::CDBFieldSelector()
  : CSetContent()
  , mTable()
  , mField()
  , mpSetContent(NULL)
{}

CDBFieldSelector::CDBFieldSelector(const CDBFieldSelector & src)
  : CSetContent(src)
  , mTable()
  , mField()
  , mpSetContent(CSetContent::copy(src.mpSetContent))
{}

CDBFieldSelector::CDBFieldSelector(const json_t * json)
  : CSetContent()
  , mTable()
  , mField()
  , mpSetContent(NULL)
{
  json_t * pValue = json_object_get(json, "elementType");

  mValid = (json_is_string(pValue) && strcmp(json_string_value(pValue), "dbField") == 0);

  if (!mValid) return;

  pValue = json_object_get(json, "table");

  if (json_is_string(pValue))
    {

    }
  else
    {
      mValid = false;
    }

  pValue = json_object_get(json, "field");

  if (json_is_string(pValue))
    {

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

CDBFieldSelector::~CDBFieldSelector()
{
  CSetContent::destroy(mpSetContent);
}

