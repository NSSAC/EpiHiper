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

#include "CSetOperation.h"

CSetOperation::CSetOperation()
{
  // TODO Auto-generated constructor stub

}

CSetOperation::~CSetOperation()
{
  // TODO Auto-generated destructor stub
}

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
