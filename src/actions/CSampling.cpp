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

#include "actions/CSampling.h"
#include "actions/CActionEnsemble.h"

CSampling::CSampling()
  : mType()
  , mpSampled(NULL)
  , mpNodeSampled(NULL)
  , mValid(false)
{}

CSampling::CSampling(const CSampling & src)
  : mType(src.mType)
  , mpSampled(src.mpSampled != NULL ? new CActionEnsemble(*src.mpSampled) : NULL)
  , mpNodeSampled(src.mpNodeSampled != NULL ? new CActionEnsemble(*src.mpNodeSampled) : NULL)
  , mValid(src.mValid)
{}

CSampling::CSampling(const json_t * json)
  : mType()
  , mpSampled(NULL)
  , mpNodeSampled(NULL)
  , mValid(false)
{
  fromJSON(json);
}

// virtual
CSampling::~CSampling()
{}

// virtual
void CSampling::fromJSON(const json_t *json)
{
  /*
    "sampling": {
      "$id": "#sampling",
      "description": "Sampling within a set with actions for sampled and non-sampled individuals.",
      "allOf": [
        {"$ref": "#/definitions/annotation"},
        {
          "oneOf": [
            {
              "$id": "#relativeSampling",
              "description": "",
              "type": "object",
              "required": [
                "type",
                "number"
              ],
              "minProperties": 3,
              "additionalProperties": false,
              "properties": {
                "type": {
                  "type": "string",
                  "enum": [
                    "individual",
                    "group"
                  ]
                },
                "number": {
                  "type": "number",
                  "minimum": 0,
                  "maximum": 100
                },
                "sampled": {"$ref": "#/definitions/actionEnsemble"},
                "nonsampled": {"$ref": "#/definitions/actionEnsemble"}
              }
            },
            {
              "$id": "#absoluteSampling",
              "description": "",
              "type": "object",
              "required": [
                "type",
                "number"
              ],
              "minProperties": 3,
              "additionalProperties": false,
              "properties": {
                "type": {
                  "type": "string",
                  "enum": ["absolute"]
                },
                "number": {
                  "type": "number",
                  "minimum": 0,
                  "multipleOf": 1.0
                },
                "sampled": {"$ref": "#/definitions/actionEnsemble"},
                "nonsampled": {"$ref": "#/definitions/actionEnsemble"}
              }
            }
          ]
        }
      ]
    },
   */
  // TODO CRITICAL Implement me!

}

void CSampling::process(const CSetContent & targets) const
{
  // TODO CRITICAL Implement me!
}

const bool & CSampling::isValid() const
{
  return mValid;
}
