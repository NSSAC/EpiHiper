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
  static CDependencyGraph::UpdateOrder UpdateSequence;

#pragma omp single
  {
    CLogger::setSingle(true);
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
    CCommunicate::roundRobinFixed(pGlobalTriggered, INSTANCES.size() * sizeof(bool), &Receive);

    {
      bool * pTriggered = pGlobalTriggered;
      std::vector< CTrigger * >::iterator it = INSTANCES.begin();
      std::vector< CTrigger * >::iterator end = INSTANCES.end();

      for (; it != end; ++it, ++pTriggered)
        {
          (*it)->trigger(*pTriggered);
        }
    }

    CDependencyGraph::getUpdateOrder(UpdateSequence, RequiredTargets);

    CLogger::setSingle(false);
  }

  return CDependencyGraph::applyUpdateOrder(UpdateSequence);
}

// static
CCommunicate::ErrorCode CTrigger::receive(std::istream & is, int /* sender */)
{
  bool Received[INSTANCES.size()];

  is.read(reinterpret_cast<char *>(Received), INSTANCES.size() * sizeof(bool));

  bool * pTriggered = pGlobalTriggered;
  bool * pReceived = Received;

  for (int i = 0; i < (int) INSTANCES.size(); ++i, ++pTriggered, ++pReceived)
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
  mIsLocalTrue = mCondition.isTrue();

  if (mIsLocalTrue)
    CLogger::info("CTrigger: Trigger '{}' condition is 'true'.", getAnnId()); 
}

void CTrigger::trigger(const bool & triggers)
{
  if (triggers 
      && !mInterventions.empty())
    {
      CLogger::info("CTrigger: Processing trigger '{}'.", getAnnId()); 
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
          CLogger::error("CTrigger ({}): Invalid value for 'trigger'.", mAnnId);
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
            CLogger::error("Trigger ({}): Invalid id for item '{}'.", mAnnId, i);
            return;
          }
      }

  mValid = true;
}

const bool & CTrigger::isValid() const
{
  return mValid;
}

