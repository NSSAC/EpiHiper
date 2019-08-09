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
}

// virtual
void CSetOperation::compute()
{
  // TODO CRITICAL Implement me!
}

