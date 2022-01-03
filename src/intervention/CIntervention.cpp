// BEGIN: Copyright 
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include <sstream>
#include <jansson.h>

#include "intervention/CIntervention.h"
#include "intervention/CTrigger.h"
#include "sets/CSetList.h"
#include "traits/CTrait.h"
#include "utilities/CSimConfig.h"
#include "variables/CVariableList.h"
#include "utilities/CLogger.h"

// static
void CIntervention::load(const std::string & file)
{
  /*
    "required": [
      "epiHiperSchema",
      "triggers",
      "interventions"
    ],
    "properties": {
      "epiHiperSchema": {
        "type": "string",
        "enum": ["https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/interventionSchema.json"]
      },
      "sets": {"$ref": "./typeRegistry.json#/definitions/sets"},
      "variables": {"$ref": "./typeRegistry.json#/definitions/variables"},
      "triggers": {"$ref": "./typeRegistry.json#/definitions/triggers"},
      "interventions": {"$ref": "./typeRegistry.json#/definitions/interventions"}
    }
   */

  json_t * pRoot = CSimConfig::loadJson(file, JSON_DECODE_INT_AS_REAL);

  CTrait::load(pRoot);

  CSetList::INSTANCE.fromJSON(json_object_get(pRoot, "sets"));
  CVariableList::INSTANCE.fromJSON(json_object_get(pRoot, "variables"));

  json_t * pArray = json_object_get(pRoot, "interventions");

  for (size_t i = 0, imax = json_array_size(pArray); i < imax; ++i)
    {
      CIntervention * pIntervention = new CIntervention(json_array_get(pArray, i));

      if (!pIntervention->isValid())
        {
          delete pIntervention;
        }
    }

  CTrigger::loadJSON(json_object_get(pRoot, "triggers"));

  json_decref(pRoot);
}

// static
void CIntervention::release()
{
  CTrigger::release();

  std::map< std::string, CIntervention * >::iterator it = INSTANCES.begin();
  std::map< std::string, CIntervention * >::iterator end = INSTANCES.end();

  for (; it != end; ++it)
    {
      delete it->second;
    }

  INSTANCES.clear();
}

// static
bool CIntervention::processAll()
{
  bool success = true;

  std::map< std::string, CIntervention * >::iterator it = INSTANCES.begin();
  std::map< std::string, CIntervention * >::iterator end = INSTANCES.end();

  for (; it != end; ++it)
    {
      success &= it->second->process();
    }

  return success;
}

// static
CIntervention * CIntervention::getIntervention(const std::string & id)
{
  std::map< std::string, CIntervention * >::iterator found = INSTANCES.find(id);

  if (found != INSTANCES.end())
    {
      return found->second;
    }

  return NULL;
}

CIntervention::CIntervention()
  : CInitialization()
  , mId()
  , mIsTriggered(false)
{}

CIntervention::CIntervention(const json_t * json)
  : CInitialization()
  , mId()
  , mIsTriggered(false)
{
  fromJSON(json);
}

CIntervention::CIntervention(const CIntervention & src)
  : CInitialization(src)
  , mId(src.mId)
  , mIsTriggered(src.mIsTriggered)
{}

// virtual
CIntervention::~CIntervention()
{}

void CIntervention::fromJSON(const json_t * json)
{
  /*
    "allOf": [
      {
        "oneOf": [
          {
            "type": "object",
            "required": ["id"],
            "properties": {
              "id": {"$ref": "#/definitions/uniqueId"}
            }
          },
          {
            "type": "object",
            "required": ["trigger"],
            "properties": {
              "trigger": {"$ref": "#/definitions/boolean"}
            }
          }
        ]
      },
      {"$ref": "#/definitions/assignmentSet"}
    ]
   */

  CInitialization::fromJSON(json);

  if (!mValid)
    return;

  if (mAnnId.find("epiHiper.initialization.") != std::string::npos)
    {
      mAnnId.clear();
    }
    
  json_t * pValue = json_object_get(json, "trigger");

  if (json_is_object(pValue))
    {
      std::ostringstream uniqueId;
      uniqueId << "epiHiper.intervention." << INSTANCES.size();
      mId = uniqueId.str();
      INSTANCES[mId] = this;

      json_object_set_new(const_cast< json_t * >(json), "interventionIds", json_array());
      json_t * pArray = json_object_get(json, "interventionIds");
      json_array_append_new(pArray, json_string(mId.c_str()));

      CTrigger * pTrigger = new CTrigger(json);

      if (pTrigger->isValid())
        {
          CTrigger::INSTANCES.push_back(pTrigger);
        }
      else
        {
          delete pTrigger;
          CLogger::error("Intervention: Invalid trigger.");
          mValid = false; // DONE
        }

      return;
    }

  mId.clear();
  pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
      mId = json_string_value(pValue);
      INSTANCES[mId] = this;

      if (CAnnotation::mAnnId.empty())
        {
          CAnnotation::mAnnId = mId;
        }
        
      return;
    }

  mValid = false; // DONE
}

bool CIntervention::process()
{
  bool success = true;
  size_t IsTriggered = 0;

# pragma omp atomic read
  IsTriggered = mIsTriggered;

  if (IsTriggered > 0)
    {
      CLogger::info() << "CIntervention: Process '" << mId << "'."; 

      success &= CInitialization::process();

#pragma omp critical
      --mIsTriggered;
    }

  return success;
}

void CIntervention::trigger()
{
  CLogger::info() << "CIntervention: Intervention '" << mId << "' has been triggered."; 

  mIsTriggered = CCommunicate::LocalProcesses();
}
