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

#include "sets/CEdgeElementSelector.h"

CEdgeElementSelector::CEdgeElementSelector()
  : CSetContent()
{}

CEdgeElementSelector::CEdgeElementSelector(const CEdgeElementSelector & src)
  : CSetContent(src)
{}

CEdgeElementSelector::CEdgeElementSelector(const json_t * json)
  : CSetContent()
{
  json_t * pValue = json_object_get(json, "elementType");

  mValid = (json_is_string(pValue) && strcmp(json_string_value(pValue), "edge") == 0);

  if (!mValid) return;

  pValue = json_object_get(json, "operator");

  mValid &= json_is_string(pValue);

  if (!mValid) return;

  if (strcmp(json_string_value(pValue), "==") == 0 ||
      strcmp(json_string_value(pValue), "!=") == 0 ||
      strcmp(json_string_value(pValue), "<=") == 0 ||
      strcmp(json_string_value(pValue), "<") == 0 ||
      strcmp(json_string_value(pValue), ">=") == 0 ||
      strcmp(json_string_value(pValue), ">") == 0)
    {
      /*
          "left": {
            "type": "object",
            "required": ["edge"],
            "properties": {
              "edge": {"$ref": "#/definitions/edgeProperty"}
            }
          },
          "right": {"$ref": "#/definitions/value"}
       */
    }
  else if (strcmp(json_string_value(pValue), "withPropertyIn") == 0)
    {
      /*
            "left": {
              "type": "object",
              "required": ["edge"],
              "properties": {
                "edge": {"$ref": "#/definitions/edgeProperty"}
              }
            },
            "right": {"$ref": "#/definitions/valueList"}
       */

    }
  else if (strcmp(json_string_value(pValue), "withTargetNodeIn") == 0 ||
      strcmp(json_string_value(pValue), "withSourceNodeIn") == 0)
    {
      /*
            "selector": {"$ref": "#/definitions/setContent"}
       */
    }
  else
    {
      mValid = false;
    }
}

CEdgeElementSelector::~CEdgeElementSelector()
{}

