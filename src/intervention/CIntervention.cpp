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

#include <sstream>
#include <jansson.h>

#include "intervention/CIntervention.h"
#include "intervention/CTrigger.h"
#include "sets/CSetList.h"
#include "utilities/CSimConfig.h"
#include "variables/CVariableList.h"

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

  CSetList::INSTANCE.fromJSON(json_object_get(pRoot, "sets"));
  CVariableList::INSTANCE.fromJSON(json_object_get(pRoot, "variables"));

  json_t * pArray = json_object_get(pRoot, "interventions");

  for (size_t i = 0, imax = json_array_size(pArray); i < imax; ++i)
    {
      CIntervention *pIntervention = new CIntervention(json_array_get(pArray, i));

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
void CIntervention::processAll()
{
  std::map< std::string, CIntervention * >::iterator it = INSTANCES.begin();
  std::map< std::string, CIntervention * >::iterator end = INSTANCES.end();

  for (; it != end; ++it)
    {
      it->second->process();
    }
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
  , mIsTriggered(false)
  , mId()
{}

CIntervention::CIntervention(const json_t * json)
  : CInitialization()
  , mIsTriggered(false)
  , mId()
{
  fromJSON(json);
}

CIntervention::CIntervention(const CIntervention & src)
  : CInitialization(src)
  , mIsTriggered(src.mIsTriggered)
  , mId(src.mId)
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

  json_t *pValue = json_object_get(json, "trigger");

  if (json_is_object(pValue))
    {
      std::ostringstream uniqueId;
      uniqueId << "epiHiper:" << INSTANCES.size();
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
          mValid = false;
        }

      return;
    }

  pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
      mId = json_string_value(pValue);
      INSTANCES[mId] = this;

      return;
    }

  mValid = false;
}

void CIntervention::process()
{
  if (!mIsTriggered)
    return;

  CInitialization::process();

  mIsTriggered = false;
}

void CIntervention::trigger()
{
  mIsTriggered = true;
}

