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

#include <jansson.h>

#include "actions/CConditionDefinition.h"

CConditionDefinition::CConditionDefinition()
  : mValid(false)
{}

CConditionDefinition::CConditionDefinition(const CConditionDefinition & src)
  : mValid(src.mValid)
{}

CConditionDefinition::CConditionDefinition(const json_t * json)
  : mValid(false)
{
  fromJSON(json);
}

// virtual
CConditionDefinition::~CConditionDefinition()
{}

void CConditionDefinition::fromJSON(const json_t * json)
{
  /*
    "condition": {
      "$id": "#condition",
      "description": "A condition used in evaluating triggers and action conditions.",
      "oneOf": [
        {
          "description": "Comparing values of nodes, edges, variables, set sizes, and fixed values",
          "type": "object",
          "required": [
            "operator",
            "left",
            "right"
          ],
          "properties": {
            "operator": {"$ref": "#/definitions/comparisonOperator"},
            "left": {
              "type": "object",
              "description": "",
              "oneOf": [
                {"$ref": "#/definitions/value"},
                {"$ref": "#/definitions/observable"},
                {
                  "type": "object",
                  "required": ["node"],
                  "properties": {
                    "node": {"$ref": "#/definitions/nodeProperty"}
                  }
                },
                {
                  "type": "object",
                  "required": ["edge"],
                  "properties": {
                    "edge": {"$ref": "#/definitions/edgeProperty"}
                  }
                },
                {"$ref": "#/definitions/variableReference"},
                {"$ref": "#/definitions/sizeof"}
              ]
            },
            "right": {
              "type": "object",
              "description": "",
              "oneOf": [
                {"$ref": "#/definitions/value"},
                {"$ref": "#/definitions/observable"},
                {
                  "type": "object",
                  "required": ["node"],
                  "properties": {
                    "node": {"$ref": "#/definitions/nodeProperty"}
                  }
                },
                {
                  "type": "object",
                  "required": ["edge"],
                  "properties": {
                    "edge": {"$ref": "#/definitions/edgeProperty"}
                  }
                },
                {"$ref": "#/definitions/variableReference"},
                {"$ref": "#/definitions/sizeof"}
              ]
            }
          }
        },
        {
          "description": "Checking whether a node or edge property is in or not in a given value list.",
          "type": "object",
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
            "left": {
              "type": "object",
              "description": "",
              "oneOf": [
                {
                  "type": "object",
                  "required": ["node"],
                  "properties": {
                    "node": {"$ref": "#/definitions/nodeProperty"}
                  }
                },
                {
                  "type": "object",
                  "required": ["edge"],
                  "properties": {
                    "edge": {"$ref": "#/definitions/edgeProperty"}
                  }
                }
              ]
            },
            "right": {"$ref": "#/definitions/valueList"}
          }
        }
      ]
    },

   */
  // TODO CRITICAL Implement me!
}

const bool & CConditionDefinition::isValid() const
{
  return mValid;
}
