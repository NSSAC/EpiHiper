// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
//  
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal 
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions: 
//  
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software. 
//  
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE 
// END: Copyright 

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
#include "variables/CVariableList.h"
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

  CTrait::load(pRoot);

  CSetList::INSTANCE.fromJSON(json_object_get(pRoot, "sets"));
  CVariableList::INSTANCE.fromJSON(json_object_get(pRoot, "variables"));

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
  static CComputable::Sequence InitializationSequence;

#pragma omp single
  {
    CLogger::setSingle(true);
    CComputableSet RequiredTargets;

    for (; it != end; ++it)
      {
        RequiredTargets.insert((*it)->getTarget());
      }

    CDependencyGraph::getUpdateSequence(InitializationSequence, RequiredTargets);
    CLogger::setSingle(false);
  }

  bool success = CDependencyGraph::applyUpdateSequence(InitializationSequence);

  for (it = INSTANCES.begin(); it != end && success; ++it)
    {
      CLogger::info() << "CInitialization: Process initialization '" << (*it)->getAnnId() << "'.";
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

  CAnnotation::fromJSON(json);

  if (mAnnId.empty())
    {
      std::ostringstream uniqueId;
      uniqueId << "epiHiper.initialization." << INSTANCES.size();
      mAnnId = uniqueId.str();
    }

  mpTarget = CSetContent::create(json_object_get(json, "target"));
  mActionEnsemble.fromJSON(json);

  mValid = (mpTarget != NULL &&
            mpTarget->isValid() &&
            mActionEnsemble.isValid());
}

bool CInitialization::process()
{
  return mActionEnsemble.process(*mpTarget);
}

CSetContent * CInitialization::getTarget()
{
  return mpTarget;
}

const bool & CInitialization::isValid() const
{
  return mValid;
}
