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

#include "sets/CNodeElementSelector.h"

CNodeElementSelector::CNodeElementSelector()
  : CSetContent()
{}

CNodeElementSelector::CNodeElementSelector(const CNodeElementSelector & src)
  : CSetContent(src)
{}

CNodeElementSelector::CNodeElementSelector(const json_t * json)
  : CSetContent()
{
  fromJSON(json);
}

CNodeElementSelector::~CNodeElementSelector()
{}

// virtual
void CNodeElementSelector::fromJSON(const json_t * json)
{
  json_t * pValue = json_object_get(json, "elementType");

  mValid = (json_is_string(pValue) && strcmp(json_string_value(pValue), "node") == 0);

  if (!mValid) return;

  pValue = json_object_get(json, "operator");

  if (strcmp(json_string_value(pValue), "==") == 0)
    {
      mpComparison = &operator==;
    }
  else if(strcmp(json_string_value(pValue), "!=") == 0)
    {
      mpComparison = &operator!=;
    }
  else if(strcmp(json_string_value(pValue), "<=") == 0)
    {
      mpComparison = &operator<=;
    }
  else if(strcmp(json_string_value(pValue), "<") == 0)
    {
      mpComparison = &operator<;
    }
  else if(strcmp(json_string_value(pValue), ">=") == 0)
    {
      mpComparison = &operator>=;
    }
  else if(strcmp(json_string_value(pValue), ">") == 0)
    {
      mpComparison = &operator>;
    }

  // Select node where the node property value comparison with the provided value is true.
  if (mpComparison != NULL)
    {
      mLeft.fromJSON(json_object_get(json, "left"));
      mValid &= mLeft.isValid();
      mpValue = new CValue(json_object_get(json, "right"));
      mValid &= (mpValue != NULL && mpValue->isValid());

      return;
    }

  // Select nodes where the node property value in in the provided list.
  if (strcmp(json_string_value(pValue), "withPropertyIn") == 0)
    {
      mLeft.fromJSON(json_object_get(json, "left"));
      mValid &= mLeft.isValid();
      mpValueList = new CValueList(json_object_get(json, "right"));
      mValid &= (mpValueList != NULL && mpValueList->isValid());

      return;
    }

  // Select nodes where the node property value in in the provided list.
  if (strcmp(json_string_value(pValue), "withIncomingEdgeIn") == 0)
    {
      // We need to identify that we have this case
      mpSetContent = CSetContent::create(json_object_get(json, "selector"));
      mValid &= (mpSetContent != NULL && mpSetContent->isValid());

      if (mValid)
        mPrerequisites.insert(mpSetContent);

      return;
    }


  /*
   // All nodes in a table
      "oneOf": [
        {
          "description": "A filter selecting nodes from the external person trait database.",
          "oneOf": [
            {
              "description": "A table in the external person trait database.",
              "type": "object",
              "required": [
                "elementType",
                "table"
              ],
              "additionalProperties": false,
              "properties": {
                "elementType": {
                  "type": "string",
                  "enum": ["node"]
                },
                "table": {"$ref": "#/definitions/uniqueIdRef"}
              }
            },


            {
              "description": "A filter returining nodes if the result of comparing left and right values with the operator is true.",
              "required": [
                "elementType",
                "operator",
                "left",
                "right"
              ],
              "properties": {
                "elementType": {
                  "type": "string",
                  "enum": ["node"]
                },
                "operator": {"$ref": "#/definitions/comparisonOperator"},
                "left": {"$ref": "#/definitions/dbField"},
                "right": {
                  "type": "object",
                  "description": "",
                  "oneOf": [
                    {"$ref": "#/definitions/dbFieldValue"},
                    {"$ref": "#/definitions/observable"}
                  ]
                }
              }
            },
            {
              "description": "A filter returining nodes if the left field value is or is not in the right list.",
              "required": [
                "elementType",
                "operator",
                "left",
                "right"
              ],
              "properties": {
                "elementType": {
                  "type": "string",
                  "enum": ["node"]
                },
                "operator": {
                  "description": "",
                  "type": "string",
                  "enum": [
                    "in",
                    "not in"
                  ]
                },
                "left": {"$ref": "#/definitions/dbField"},
                "right": {
                  "type": "object",
                  "description": "",
                  "oneOf": [
                    {"$ref": "#/definitions/dbFieldValueList"},
                    {"$ref": "#/definitions/dbFieldValueSelector"}
                  ]
                }
              }
            }
          ]
        }
      ]
 */
}

// virtual
void CNodeElementSelector::compute()
{
  // TODO CRITICAL Implement me!
}


