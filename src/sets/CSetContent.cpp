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

#include "CSetContent.h"

CSetContent::CSetContent()
{
  // TODO Auto-generated constructor stub

}

CSetContent::~CSetContent()
{
  // TODO Auto-generated destructor stub
}

/*
      "oneOf": [
        {"$ref": "#/definitions/edgeElementSelector"},
        {"$ref": "#/definitions/nodeElementSelector"},
        {"$ref": "#/definitions/dbFieldValueSelector"},
        {"$ref": "#/definitions/setOperation"},
        {"$ref": "#/definitions/setReference"},
        {
          "description": "Either all nodes or all edges",
          "type": "object",
          "required": ["elementType"],
          "properties": {
            "elementType": {
              "type": "string",
              "enum": [
                "node",
                "edge"
              ]
            }
          },
          "additionalProperties": false
        }
      ]

 */
