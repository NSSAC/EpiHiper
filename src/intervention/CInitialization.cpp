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

#include "intervention/CInitialization.h"
#include "utilities/CSimConfig.h"
#include "sets/CSetList.h"
#include "sets/CSetContent.h"
#include "math/CDependencyGraph.h"
#include "traits/CTrait.h"
#include "utilities/CLogger.h"

// static
void CInitialization::load(const std::string & file)
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

  json_t * pRoot = CSimConfig::loadJson(file, JSON_DECODE_INT_AS_REAL);
  CSetList::INSTANCE.fromJSON(json_object_get(pRoot, "sets"));

  // Get the key "traits"
  json_t * pTraits = json_object_get(pRoot, "traits");

  // Iterate of the array elements
  for (size_t i = 0, imax = json_array_size(pTraits); i < imax; ++i)
    {
      CTrait Trait;
      Trait.fromJSON(json_array_get(pTraits, i));

      if (Trait.isValid())
        {
          CTrait::INSTANCES[Trait.getId()] = Trait;
        }
    }

  json_t * pArray = json_object_get(pRoot, "initializations");

  for (size_t i = 0, imax = json_array_size(pArray); i < imax; ++i)
    {
      CInitialization *pInitialization = new CInitialization(json_array_get(pArray, i));

      if (pInitialization->isValid())
        {
          INSTANCES.push_back(pInitialization);
        }
      else
        {
          delete pInitialization;
        }
    }

  json_decref(pRoot);
}

// static
void CInitialization::release()
{
  std::vector< CInitialization * >::iterator it = INSTANCES.begin();
  std::vector< CInitialization * >::iterator end = INSTANCES.end();

  for (; it != end; ++it)
    {
      delete *it;
    }

  INSTANCES.clear();
}

// static
bool CInitialization::processAll()
{
  std::vector< CInitialization * >::iterator it = INSTANCES.begin();
  std::vector< CInitialization * >::iterator end = INSTANCES.end();

  CComputableSet RequiredTargets;

  for (; it != end; ++it)
    {
      RequiredTargets.insert((*it)->getTarget());
    }

  CComputable::Sequence InitializationSequence;
  CDependencyGraph::getUpdateSequence(InitializationSequence, RequiredTargets);
  bool success = CDependencyGraph::applyUpdateSequence(InitializationSequence);

  for (it = INSTANCES.begin(); it != end && success; ++it)
    {
      CLogger::debug() << "CInitialization: Process initialization '" << (*it)->getAnnId() << "'.";
      (*it)->process();
    }

  return success;
}

CInitialization::CInitialization()
  : CAnnotation()
  , mpTarget(NULL)
  , mActionEnsemble()
  , mValid(false)
{}

CInitialization::CInitialization(const CInitialization & src)
  : CAnnotation(src)
  , mpTarget(src.mpTarget != NULL ? src.mpTarget->copy() : NULL)
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

void CInitialization::process()
{
  mActionEnsemble.process(*mpTarget);
}

CSetContent * CInitialization::getTarget()
{
  return mpTarget;
}

const bool & CInitialization::isValid() const
{
  return mValid;
}
