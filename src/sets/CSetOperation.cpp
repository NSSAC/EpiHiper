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

#include "sets/CSetOperation.h"

CSetOperation::CSetOperation()
  : CSetContent()
{}

CSetOperation::CSetOperation(const CSetOperation & src)
  : CSetContent(src)
{}

CSetOperation::CSetOperation(const json_t * json)
  : CSetContent()
{
  fromJSON(json);
}


CSetOperation::~CSetOperation()
{}

// virtual
void CSetOperation::fromJSON(const json_t * json)
{
  /*
    "required": [
      "operation",
      "sets"
    ],
    "properties": {
      "operation": {
        "type": "string",
        "enum": [
          "union",
          "intersection"
        ]
      },
      "sets": {
        "type": "array",
        "items": {"$ref": "#/definitions/setContent"}
      }
    },
  */

  json_t * pValue = json_object_get(json, "operation");

  if (!json_is_string(pValue))
    {
      mValid = false;
      return;
    }

  if (strcmp(json_string_value(pValue), "union") == 0)
    {
      mpCompute = &CSetOperation::computeUnion;
    }
  else if (strcmp(json_string_value(pValue), "intersection") == 0)
    {
      mpCompute = &CSetOperation::computeIntersection;
    }
  else
    {
      mValid = false;
      return;
    }

  pValue = json_object_get(json, "sets");

  if (!json_is_array(pValue))
    {
      mValid = false;
      return;
    }

}

// virtual
void CSetOperation::compute()
{
  if (mValid &&
      mpCompute != NULL)
    (this->*mpCompute)();
}

void CSetOperation::computeUnion()
{

}

void CSetOperation::computeIntersection()
{

}

