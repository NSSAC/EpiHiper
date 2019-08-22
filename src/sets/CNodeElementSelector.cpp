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
  , mLeft()
  , mpValue(NULL)
  , mpValueList(NULL)
  , mpSetContent(NULL)
  , mpComparison(NULL)
  , mSQLComparison("")
  , mLocalScope(true)
{}

CNodeElementSelector::CNodeElementSelector(const CNodeElementSelector & src)
  : CSetContent(src)
  , mLeft(src.mLeft)
  , mpValue(src.mpValue != NULL ? new CValue(*src.mpValue) : NULL)
  , mpValueList(src.mpValueList != NULL ? new CValueList(*src.mpValueList) : NULL)
  , mpSetContent(CSetContent::copy(src.mpSetContent))
  , mpComparison(src.mpComparison)
  , mSQLComparison(src.mSQLComparison)
  , mLocalScope(src.mLocalScope)
{}

CNodeElementSelector::CNodeElementSelector(const json_t * json)
  : CSetContent()
  , mLeft()
  , mpValue(NULL)
  , mpValueList(NULL)
  , mpSetContent(NULL)
  , mpComparison(NULL)
  , mSQLComparison("")
  , mLocalScope(true)
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

  pValue = json_object_get(json, "scope");

  if (strcmp(json_string_value(pValue), "local") == 0)
    {
      mLocalScope = true;
    }
  else if(strcmp(json_string_value(pValue), "global") == 0)
    {
      mLocalScope = false;
    }
  else
    {
      mValid = false;
    }

  pValue = json_object_get(json, "operator");

  if (strcmp(json_string_value(pValue), "==") == 0)
    {
      mpComparison = &operator==;
      mSQLComparison = "=";
    }
  else if(strcmp(json_string_value(pValue), "!=") == 0)
    {
      mpComparison = &operator!=;
      mSQLComparison = "<>";
    }
  else if(strcmp(json_string_value(pValue), "<=") == 0)
    {
      mpComparison = &operator<=;
      mSQLComparison = "<=";
    }
  else if(strcmp(json_string_value(pValue), "<") == 0)
    {
      mpComparison = &operator<;
      mSQLComparison = "<";
    }
  else if(strcmp(json_string_value(pValue), ">=") == 0)
    {
      mpComparison = &operator>=;
      mSQLComparison = ">=";
    }
  else if(strcmp(json_string_value(pValue), ">") == 0)
    {
      mpComparison = &operator>;
      mSQLComparison = ">";
    }

  // Select node where the node property value comparison with the provided value is true.
  if (mpComparison != NULL)
    {
      /*
        {
          "required": [
            "operator",
            "left",
            "right"
          ],
          "properties": {
            "operator": {"$ref": "#/definitions/comparisonOperator"},
            "left": {
              "type": "object",
              "required": ["node"],
              "properties": {
                "node": {"$ref": "#/definitions/nodeProperty"}
              }
            },
            "right": {"$ref": "#/definitions/value"}
          }
        }
      */

      mLeft.fromJSON(json_object_get(json, "left"));

      if (mLeft.isValid())
        {
          mpValue = new CValue(json_object_get(json, "right"));
          mValid &= (mpValue != NULL && mpValue->isValid());

          if (mValid) return;
        }

      /*
        {
          "description": "A filter returning nodes if the result of comparing left and right values with the operator is true.",
          "required": [
            "operator",
            "left",
            "right"
          ],
          "properties": {
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
      */

      mDBField.fromJSON(json_object_get(json, "left"));

      if (mDBField.isValid())
        {

        }

      return;
    }

  // Select nodes where the node property value in in the provided list.
  if (strcmp(json_string_value(pValue), "withPropertyIn") == 0)
    {
      /*
        {
          "description": "",
          "required": [
            "operator",
            "left",
            "right"
          ],
          "properties": {
            "operator": {
              "description": "",
              "type": "string",
              "enum": ["withPropertyIn"]
            },
            "left": {
              "type": "object",
              "required": ["node"],
              "properties": {
                "node": {"$ref": "#/definitions/nodeProperty"}
              }
            },
            "right": {"$ref": "#/definitions/valueList"}
          }
        },
      */

      mLeft.fromJSON(json_object_get(json, "left"));
      mValid &= mLeft.isValid();
      mpValueList = new CValueList(json_object_get(json, "right"));
      mValid &= (mpValueList != NULL && mpValueList->isValid());

      return;
    }

  // Select nodes where the node property value in in the provided list.
  if (strcmp(json_string_value(pValue), "withIncomingEdgeIn") == 0)
    {
      /*
        {
          "description": "",
          "required": [
            "operator",
            "selector"
          ],
          "properties": {
            "operator": {
              "description": "",
              "type": "string",
              "enum": ["withIncomingEdgeIn"]
            },
            "selector": {"$ref": "#/definitions/setContent"}
          }
        },
      */
      // We need to identify that we have this case
      mpSetContent = CSetContent::create(json_object_get(json, "selector"));
      mValid &= (mpSetContent != NULL && mpSetContent->isValid());

      if (mValid)
        mPrerequisites.insert(mpSetContent);

      return;
    }


  /*
"nodeElementSelector": {
      "$id": "#nodeElementSelector",
      "description": "The specification of node elements of a set.",
      "type": "object",
      "allOf": [
            {
              "description": "A filter selecting nodes from the external person trait database.",
              "oneOf": [
                {
                  "description": "A table in the external person trait database.",
                  "type": "object",
                  "required": ["table"],
                  "properties": {
                    "table": {"$ref": "#/definitions/uniqueIdRef"}
                  }
                },
                {
                  "description": "A filter returning nodes if the left field value is or is not in the right list.",
                  "required": [
                    "operator",
                    "left",
                    "right"
                  ],
                  "properties": {
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
        }
      ]
    } */
}

// virtual
void CNodeElementSelector::compute()
{
  // TODO CRITICAL Implement me!
}


