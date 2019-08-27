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

#include "math/CObservable.h"


/*
CObservable::CObservable()
  : CComputable()
  , mValid(false)
{}
*/

CObservable::CObservable(const CObservable & src)
  : CValueInterface(src)
  , CComputable(src)
  , mValid(src.mValid)
{}

/*
CObservable::CObservable(const json_t * json)
  : CValueInterface(src)
  , CComputable()
  , mValid(false)
{
  fromJSON(json);
}
*/

// virtual
CObservable::~CObservable()
{
  // TODO Auto-generated destructor stub
}


// virtual
void CObservable::compute()
{
  // TODO CRITICAL Implement me!
}

void CObservable::fromJSON(const json_t * json)
{
  /*
    "observable": {
      "$id": "#observable",
      "description": "An observable which is always accessible within EpiHiper (time and health state statistics) of the current state of the system.",
      "type": "object",
      "required": ["observable"],
      "properties": {
        "observable": {
          "oneOf": [
            {
              "type": "object",
              "required": [
                "healthState",
                "type"
              ],
              "properties": {
                "healthState": {"$ref": "#/definitions/uniqueIdRef"},
                "type": {
                  "type": "string",
                  "enum": [
                    "absolute",
                    "relative"
                  ]
                }
              }
            },
            {
              "description": "The time property references the current simulation time.",
              "type": "string",
              "enum": ["time"]
            }
          ]
        }
      }
    },
  */
  // TODO CRITICAL Implement me!
}

const bool & CObservable::isValid() const
{
  return mValid;
}
