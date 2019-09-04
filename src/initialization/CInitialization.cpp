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

/*
 * CInitialization.cpp
 *
 *  Created on: Aug 29, 2019
 *      Author: shoops
 */

#include <jansson.h>

#include "initialization/CInitialization.h"

#include "utilities/CSimConfig.h"
#include "sets/CSetList.h"
#include "sets/CSetContent.h"

// static
std::vector< CInitialization > CInitialization::INSTANCES;

// static
void CInitialization::init()
{
  /*
    {
      "$schema": "http://json-schema.org/draft-07/schema#",
      "title": "Initialization File",
      "$id": "#initializationFile",
      "description": "A schema describing the contents of an initialization file for EpiHiper",
      "type": "object",
      "required": [
        "epiHiperSchema",
        "initializations"
      ],
      "properties": {
        "epiHiperSchema": {
          "type": "string",
          "enum": ["https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/initializationSchema.json"]
        },
        "sets": {"$ref": "./typeRegistry.json#/definitions/sets"},
        "initializations": {"$ref": "./typeRegistry.json#/definitions/initializations"}
      }
    }
  */

  json_t * pRoot = CSimConfig::loadJson(CSimConfig::getInitialization(), JSON_DECODE_INT_AS_REAL);
  CSetList::INSTANCE.fromJSON(json_object_get(pRoot, "sets"));

  json_t * pArray = json_object_get(pRoot, "initializations");

  for (size_t i = 0, imax = json_array_size(pArray); i < imax; ++i)
    {
      CInitialization Initialization(json_array_get(pArray, i));
      INSTANCES.push_back(Initialization);
    }

  json_decref(pRoot);
}

CInitialization::CInitialization()
  : CAnnotation()
  , mpTarget(NULL)
  , mActionEnsemble()
  , mValid(false)
{}

CInitialization::CInitialization(const CInitialization & src)
  : CAnnotation(src)
  , mpTarget(CSetContent::copy(src.mpTarget))
  , mActionEnsemble(src.mActionEnsemble)
  , mValid(src.mValid)
{}

CInitialization::CInitialization(const json_t * json)
  : CAnnotation()
  , mpTarget(NULL)
  , mActionEnsemble()
  , mValid(false)
{
  fromJSON(json);
}

// virtual
CInitialization::~CInitialization()
{
  if (mpTarget != NULL)
    delete mpTarget;
}

void CInitialization::fromJSON(const json_t * json)
{
  /*
    "assignmentSet": {
      "$id": "#assignmentSet",
      "description": "A set of object with an associated action ensemble",
      "type": "object",
      "allOf": [
        {"$ref": "#/definitions/annotation"},
        {
          "$id": "#target",
          "description": "Target for set content",
          "type": "object",
          "required": ["target"],
          "properties": {
            "target": {
              "allOf": [
                {"$ref": "#/definitions/annotation"},
                {"$ref": "#/definitions/setContent"}
              ]
            }
          }
        },
        {"$ref": "#/definitions/actionEnsemble"}
      ]
    },
  */

  mpTarget = CSetContent::create(json_object_get(json, "target"));
  mActionEnsemble.fromJSON(json);

  mValid = (mpTarget != NULL &&
            mpTarget->isValid() &&
            mActionEnsemble.isValid());

  CAnnotation::fromJSON(json);
}

const bool & CInitialization::isValid() const
{
  return mValid;
}
