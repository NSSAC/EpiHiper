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

#include "CDBFieldSelector.h"

CDBFieldSelector::CDBFieldSelector()
{
  // TODO Auto-generated constructor stub

}

CDBFieldSelector::~CDBFieldSelector()
{
  // TODO Auto-generated destructor stub
}

/*
      "required": [
        "elementType",
        "table",
        "field",
        "selector"
      ],
      "properties": {
        "elementType": {
          "type": "string",
          "enum": ["dbField"]
        },
        "table": {"$ref": "#/definitions/uniqueIdRef"},
        "field": {"$ref": "#/definitions/uniqueIdRef"},
        "selector": {"$ref": "#/definitions/setContent"}
      }

 */
