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

#include "actions/CActionDefinition.h"

CActionDefinition::CActionDefinition()
  : CAnnotation()
  , mOperations()
  , mPriority(1.0)
  , mDelay(0)
  , mCondition()
  , mValid(false)
{}

CActionDefinition::CActionDefinition(const CActionDefinition & src)
  : CAnnotation(src)
  , mOperations(src.mOperations)
  , mPriority(src.mPriority)
  , mDelay(src.mDelay)
  , mCondition(src.mCondition)
  , mValid(src.mValid)
{}

CActionDefinition::CActionDefinition(const json_t * json)
  : CAnnotation()
  , mOperations()
  , mPriority(1.0)
  , mDelay(0)
  , mCondition()
  , mValid(false)
{
  fromJSON(json);
}

// virtual
CActionDefinition::~CActionDefinition()
{}

void CActionDefinition::fromJSON(const json_t * json)
{
  /*
    "action": {
      "$id": "#action",
      "description": "An action to be executed.",
      "allOf": [
        {
          "type": "object",
          "required": ["operations"],
          "properties": {
            "priority": {"$ref": "#/definitions/nonNegativeNumber"},
            "delay": {"$ref": "#/definitions/nonNegativeNumber"},
            "condition": {"$ref": "#/definitions/boolean"},

   // TODO checkConditionInterval is not implemented.
            "checkConditionInterval": {
              "description": "",
              "type": "object",
              "required": [
                "min",
                "max"
              ],
              "properties": {
                "min": {"$ref": "#/definitions/nonNegativeNumber"},
                "max": {"$ref": "#/definitions/nonNegativeNumber"}
              }
            },
            "operations": {
              "description": "",
              "type": "array",
              "minItems": 1,
              "items": {
                "description": "",
                "allOf": [
                  {"$ref": "#/definitions/annotation"},
                  {
                    "description": "",
                    "type": "object",
                    "required": ["target"],
                    "properties": {
                      "target": {
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
                          },
                          {"$ref": "#/definitions/variableReference"}
                        ]
                      }
                    }
                  },
                  {
                    "description": "",
                    "type": "object",
                    "required": ["operator"],
                    "properties": {
                      "operator": {
                        "type": "string",
                        "enum": [
                          "=",
                          "*=",
                          "/=",
                          "+=",
                          "-="
                        ]
                      }
                    }
                  },
                  {"$ref": "#/definitions/value"}
                ]
              }
            }
          },
          "patternProperties": {
            "^ann:": {}
          },
          "additionalProperties": false
        }
      ]
    },

   */

  mValid = true;

  json_t * pValue = json_object_get(json, "operations");

  for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i)
    {
      COperationDefinition OperationDefinition(json_array_get(pValue, i));
      mValid &= OperationDefinition.isValid();
      mOperations.push_back(OperationDefinition);
    }

  pValue = json_object_get(json, "priority");

  if (json_is_real(pValue))
    {
      mPriority = json_real_value(pValue);
    }

  pValue = json_object_get(json, "delay");

  if (json_is_real(pValue))
    {
      mDelay = json_real_value(pValue);
    }

  pValue = json_object_get(json, "condition");

  if (json_is_object(pValue))
    {
      mCondition.fromJSON(pValue);
      mValid &= mCondition.isValid();
    }

  CAnnotation::fromJSON(json);
}

const bool & CActionDefinition::isValid() const
{
  return mValid;
}
