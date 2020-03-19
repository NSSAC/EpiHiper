// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include <jansson.h>

#include "intervention/CTrigger.h"
#include "intervention/CIntervention.h"
#include "utilities/CSimConfig.h"
#include "actions/CCondition.h"
#include "math/CDependencyGraph.h"
#include "utilities/CLogger.h"

// static
void CTrigger::loadJSON(const json_t * json)
{
  /*
    "triggers": {
      "type": "array",
      "items": {"$ref": "#/definitions/trigger"}
    },
   */

  for (size_t i = 0, imax = json_array_size(json); i < imax; ++i)
    {
      CTrigger * pTrigger = new CTrigger(json_array_get(json, i));

      if (pTrigger->isValid())
        {
          INSTANCES.push_back(pTrigger);
        }
      else
        {
          delete pTrigger;
        }
    }
}

// static
void CTrigger::release()
{
  std::vector< CTrigger * >::iterator it = INSTANCES.begin();
  std::vector< CTrigger * >::iterator end = INSTANCES.end();

  for (; it != end; ++it)
    {
      delete *it;
    }

  INSTANCES.clear();

  if (pGlobalTriggered != NULL)
    {
      delete [] pGlobalTriggered;
    }
}

// static
bool CTrigger::processAll()
{
  RequiredTargets.clear();

  if (pGlobalTriggered == NULL)
    {
      pGlobalTriggered = new bool[INSTANCES.size()];
    }

  {
    bool * pTriggered = pGlobalTriggered;
    std::vector< CTrigger * >::const_iterator it = INSTANCES.begin();
    std::vector< CTrigger * >::const_iterator end = INSTANCES.end();

    for (; it != end; ++it, ++pTriggered)
      {
        (*it)->process();
        *pTriggered = (*it)->mIsLocalTrue;
      }
  }

  CCommunicate::Receive Receive(receive);
  CCommunicate::broadcast(pGlobalTriggered, INSTANCES.size() * sizeof(bool), &Receive);

  {
    bool * pTriggered = pGlobalTriggered;
    std::vector< CTrigger * >::iterator it = INSTANCES.begin();
    std::vector< CTrigger * >::iterator end = INSTANCES.end();

    for (; it != end; ++it, ++pTriggered)
      {
        (*it)->trigger(*pTriggered);
      }
  }

  CComputable::Sequence UpdateSequence;
  CDependencyGraph::getUpdateSequence(UpdateSequence, RequiredTargets);
  return CDependencyGraph::applyUpdateSequence(UpdateSequence);
}

// static
CCommunicate::ErrorCode CTrigger::receive(std::istream & is, int sender)
{
  bool Received[INSTANCES.size()];

  is.read(reinterpret_cast<char *>(Received), INSTANCES.size() * sizeof(bool));

  bool * pTriggered = pGlobalTriggered;
  bool * pReceived = Received;

  for (int i = 0; i < INSTANCES.size(); ++i, ++pTriggered, ++pReceived)
    {
      *pTriggered |= *pReceived;
    }

  return CCommunicate::ErrorCode::Success;
}

CTrigger::CTrigger()
  : CAnnotation()
  , mCondition()
  , mInterventions()
  , mIsLocalTrue(false)
  , mValid(false)
{}

CTrigger::CTrigger(const CTrigger & src)
  : CAnnotation(src)
  , mCondition(src.mCondition)
  , mInterventions(src.mInterventions)
  , mIsLocalTrue(src.mIsLocalTrue)
  , mValid(src.mValid)
{}

CTrigger::CTrigger(const json_t * json)
  : CAnnotation()
  , mCondition()
  , mInterventions()
  , mIsLocalTrue(false)
  , mValid(false)
{
  fromJSON(json);
}

// virtual
CTrigger::~CTrigger()
{}

// virtual
void CTrigger::process()
{
  CCondition *pCondition = mCondition.createCondition((CNode *) NULL);
  mIsLocalTrue = pCondition->isTrue();
  delete pCondition;

  CLogger::info() << "CTrigger: Trigger '" << getAnnId() << "' condition is '" << (mIsLocalTrue ? "true" : "false") << "'."; 
}

void CTrigger::trigger(const bool & triggers)
{
  if (triggers 
      && !mInterventions.empty())
    {
      CLogger::info() << "CTrigger: Processing trigger '" << getAnnId() << "'."; 
      std::map< std::string, CIntervention * >::iterator it = mInterventions.begin();
      std::map< std::string, CIntervention * >::iterator end = mInterventions.end();

      for (; it != end; ++it)
        {
          it->second->trigger();
          RequiredTargets.insert(it->second->getTarget());
        }
    }
}

// virtual
void CTrigger::fromJSON(const json_t * json)
{
  /*
  "trigger": {
    "$id": "#trigger",
    "description": "A trigger evaluating to true or false.",
    "type": "object",
    "allOf": [
      {"$ref": "#/definitions/annotation"},
      {
        "required": ["trigger"],
        "properties": {
          "trigger": {"$ref": "#/definitions/boolean"},
          "interventionIds": {
            "type": "array",
            "items": {"$ref": "#/definitions/uniqueIdRef"},
            "uniqueItems": true
          }
        },
        "patternProperties": {
          "^ann:": {}
        },
        "additionalProperties": false
      }
    ]
  },
  */
  mValid = false; // DONE
  CAnnotation::fromJSON(json);

  if (mAnnId.empty())
    {
      std::ostringstream os;
      os << "epiHiper.trigger." << INSTANCES.size();
      mAnnId = os.str();
    }

  json_t * pValue = json_object_get(json, "trigger");

  if (json_is_object(pValue))
    {
      mCondition.fromJSON(pValue);

      if (!mCondition.isValid())
        {
          CLogger::error() << "CTrigger (" << mAnnId << "): Invalid value for 'trigger'.";
          return;
        }
    }

  pValue = json_object_get(json, "interventionIds");

  for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i)
    if (json_is_string(json_array_get(pValue, i)))
      {
        std::string Id = json_string_value(json_array_get(pValue, i));
        mInterventions[Id] = CIntervention::getIntervention(Id);

        if (mInterventions[Id] == NULL)
          {
            CLogger::error() << "Trigger (" << mAnnId << "): Invalid id for item '" << i << "'.";
            return;
          }
      }

  mValid = true;
}

const bool & CTrigger::isValid() const
{
  return mValid;
}

