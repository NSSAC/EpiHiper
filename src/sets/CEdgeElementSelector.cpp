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

#include "CEdgeElementSelector.h"

CEdgeElementSelector::CEdgeElementSelector()
{
  // TODO Auto-generated constructor stub

}

CEdgeElementSelector::~CEdgeElementSelector()
{
  // TODO Auto-generated destructor stub
}

/*
      "allOf": [
        {
          "required": ["elementType"],
          "properties": {
            "elementType": {
              "type": "string",
              "enum": ["edge"]
            }
          }
        },
        {
          "oneOf": [
            {
              "description": "",
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
                  "required": ["edge"],
                  "properties": {
                    "edge": {"$ref": "#/definitions/edgeProperty"}
                  }
                },
                "right": {"$ref": "#/definitions/value"}
              }
            },
            {
              "description": "",
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
                  "enum": ["withPropertyIn"]
                },
                "left": {
                  "type": "object",
                  "required": ["edge"],
                  "properties": {
                    "edge": {"$ref": "#/definitions/edgeProperty"}
                  }
                },
                "right": {"$ref": "#/definitions/valueList"}
              }
            },
            {
              "description": "",
              "type": "object",
              "required": [
                "operator",
                "selector"
              ],
              "properties": {
                "operator": {
                  "description": "",
                  "type": "string",
                  "enum": [
                    "withTargetNodeIn",
                    "withSourceNodeIn"
                  ]
                },
                "selector": {"$ref": "#/definitions/setContent"}
              }
            }
          ]
        }
      ]

 */
