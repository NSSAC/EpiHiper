// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2023 Rector and Visitors of the University of Virginia 
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

  CActionDefinition::convertPrioritiesToOrder();
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

  // Delete auto generated annotation ids for initialization.
  if (mAnnId.find("epiHiper.initialization.") != std::string::npos)
    mAnnId.clear();
  
  // We must have one of id or trigger
  bool IdOrTrigger = false;
  json_t * pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
      mId = json_string_value(pValue);
      IdOrTrigger = true;
    }
  else if (!mAnnId.empty())
    {
      mId = mAnnId;
    }
  else
    {
      std::ostringstream uniqueId;
      uniqueId << "epiHiper.intervention." << INSTANCES.size();
      mId = uniqueId.str();
    }

  INSTANCES[mId] = this;

  if (CAnnotation::mAnnId.empty())
    CAnnotation::mAnnId = mId;

  pValue = json_object_get(json, "trigger");

  if (json_is_object(pValue))
    {
      // Added the intervention's id to the triggered ids.
      json_object_set_new(const_cast< json_t * >(json), "interventionIds", json_array());
      json_t * pArray = json_object_get(json, "interventionIds");
      json_array_append_new(pArray, json_string(mId.c_str()));

      CTrigger * pTrigger = new CTrigger(json);

      if (pTrigger->isValid())
        {
          CTrigger::INSTANCES.push_back(pTrigger);
          IdOrTrigger = true;
        }
      else
        {
          delete pTrigger;
          CLogger::error("Intervention: Invalid trigger.");
          mValid = false; // DONE
        }
    }

  mValid &= IdOrTrigger; // DONE
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
