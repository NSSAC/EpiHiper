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

#include <limits>
#include <jansson.h>

#include "actions/CActionDefinition.h"
#include "actions/CAction.h"
#include "actions/CActionQueue.h"
#include "network/CNetwork.h"
#include "network/CEdge.h"
#include "network/CNode.h"
#include "utilities/CLogger.h"

// static
CActionDefinition * CActionDefinition::GetActionDefinition(const size_t & index)
{
  if (index < INSTANCES.size())
    return INSTANCES[index];

  return NULL;
}

CActionDefinition::CActionDefinition()
  : CAnnotation()
  , mOperations()
  , mPriority(1.0)
  , mDelay(0)
  , mCondition()
  , mIndex(std::numeric_limits< size_t >::max())
  , mValid(false)
  , mInfo()
{}

CActionDefinition::CActionDefinition(const CActionDefinition & src)
  : CAnnotation(src)
  , mOperations(src.mOperations)
  , mPriority(src.mPriority)
  , mDelay(src.mDelay)
  , mCondition(src.mCondition)
  , mIndex(src.mIndex)
  , mValid(src.mValid)
  , mInfo(src.mInfo)
{}

CActionDefinition::CActionDefinition(const json_t * json)
  : CAnnotation()
  , mOperations()
  , mPriority(1.0)
  , mDelay(0)
  , mCondition()
  , mIndex(std::numeric_limits< size_t >::max())
  , mValid(false)
  , mInfo()
{
  fromJSON(json);

  if (mValid)
    {
      mIndex = INSTANCES.size();
      mInfo.set("CActionDefinition", (int) mIndex);
      INSTANCES.push_back(this);
    }
}

// virtual
CActionDefinition::~CActionDefinition()
{}

void CActionDefinition::fromJSON(const json_t * json)
{
  /*
    "action": {
      "$id": "#action",
      "description": "An action to be executed.",
      "allOf": [
        {
          "type": "object",
          "required": ["operations"],
          "properties": {
            "priority": {"$ref": "#/definitions/nonNegativeNumber"},
            "delay": {"$ref": "#/definitions/nonNegativeNumber"},
            "condition": {"$ref": "#/definitions/boolean"},

   // TODO checkConditionInterval is not implemented.
            "checkConditionInterval": {
              "description": "",
              "type": "object",
              "required": [
                "min",
                "max"
              ],
              "properties": {
                "min": {"$ref": "#/definitions/nonNegativeNumber"},
                "max": {"$ref": "#/definitions/nonNegativeNumber"}
              }
            },
            "operations": {
              "description": "",
              "type": "array",
              "minItems": 1,
              "items": {
                "description": "",
                "allOf": [
                  {"$ref": "#/definitions/annotation"},
                  {
                    "description": "",
                    "type": "object",
                    "required": ["target"],
                    "properties": {
                      "target": {
                        "oneOf": [
                          {
                            "type": "object",
                            "required": ["node"],
                            "properties": {
                              "node": {"$ref": "#/definitions/nodeProperty"}
                            }
                          },
                          {
                            "type": "object",
                            "required": ["edge"],
                            "properties": {
                              "edge": {"$ref": "#/definitions/edgeProperty"}
                            }
                          },
                          {"$ref": "#/definitions/variableReference"}
                        ]
                      }
                    }
                  },
                  {
                    "description": "",
                    "type": "object",
                    "required": ["operator"],
                    "properties": {
                      "operator": {
                        "type": "string",
                        "enum": [
                          "=",
                          "*=",
                          "/=",
                          "+=",
                          "-="
                        ]
                      }
                    }
                  },
                  {"$ref": "#/definitions/value"}
                ]
              }
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

  mValid = false;

  json_t * pValue = json_object_get(json, "operations");

  for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i)
    {
      COperationDefinition OperationDefinition(json_array_get(pValue, i));

      if (!OperationDefinition.isValid())
        {
          CLogger::error() << "Action : Invalid value for opeartions item '" << i << "'.";
          return;
        }

      mOperations.push_back(OperationDefinition);
    }

  pValue = json_object_get(json, "priority");

  if (json_is_real(pValue))
    {
      mPriority = json_real_value(pValue);
    }

  pValue = json_object_get(json, "delay");

  if (json_is_real(pValue))
    {
      mDelay = json_real_value(pValue);
    }

  pValue = json_object_get(json, "condition");

  if (json_is_object(pValue))
    {
      mCondition.fromJSON(pValue);

      if (!mCondition.isValid())
        {
          CLogger::error("Action: Invalid 'condition'.");
          return;
        }
    }

  CAnnotation::fromJSON(json);
  mValid = true;
}

const bool & CActionDefinition::isValid() const
{
  return mValid;
}

void CActionDefinition::process() const
{
  try
    {
      CAction * pAction = new CAction(mPriority, mCondition.createCondition());

      // Loop through the operation definitions
      std::vector< COperationDefinition >::const_iterator it = mOperations.begin();
      std::vector< COperationDefinition >::const_iterator end = mOperations.end();

      for (; it != end; ++it)
        {
          pAction->addOperation(it->createOperation(mInfo));
        }

      CLogger::trace() << "CActionDefinition: Add node and edge independent action.";
      CActionQueue::addAction(mDelay, pAction);
    }
  catch (...)
    {
      CLogger::error() << "CActionDefinition:: Failed to create node and edge independent action.";
    }
}

void CActionDefinition::process(const CEdge * pEdge) const
{
  if (pEdge == NULL)
    return;

  if (CNetwork::INSTANCE->isRemoteNode(pEdge->pTarget))
    {
      CLogger::trace() << "CActionDefinition: Add remote action for edge `" << pEdge->targetId << "," << pEdge->sourceId << "'.";
      CActionQueue::addRemoteAction(mIndex, pEdge);
      return;
    }

  try
    {
      CAction * pAction = new CAction(mPriority, mCondition.createCondition(pEdge));

      // Loop through the operation definitions
      std::vector< COperationDefinition >::const_iterator it = mOperations.begin();
      std::vector< COperationDefinition >::const_iterator end = mOperations.end();

      for (; it != end; ++it)
        {
          pAction->addOperation(it->createOperation(const_cast< CEdge * >(pEdge), mInfo));
        }

      CLogger::trace() << "CActionDefinition: Add action for edge `" << pEdge->targetId << "," << pEdge->sourceId << "'.";
      CActionQueue::addAction(mDelay, pAction);
    }
  catch (...)
    {
      CLogger::error() << "CActionDefinition: Failed to create action for edge `" << pEdge->targetId << "," << pEdge->sourceId << "'.";
    }
}

void CActionDefinition::process(const CNode * pNode) const
{
  if (pNode == NULL)
    return;

  if (CNetwork::INSTANCE->isRemoteNode(pNode))
    {
      CLogger::trace() << "CActionDefinition: Add remote action for node `" << pNode->id << "'.";
      CActionQueue::addRemoteAction(mIndex, pNode);
      return;
    }

  try
    {
      CAction * pAction = new CAction(mPriority, mCondition.createCondition(pNode));

      // Loop through the operation definitions
      std::vector< COperationDefinition >::const_iterator it = mOperations.begin();
      std::vector< COperationDefinition >::const_iterator end = mOperations.end();

      for (; it != end; ++it)
        {
          pAction->addOperation(it->createOperation(const_cast< CNode * >(pNode), mInfo));
        }

      CLogger::trace() << "CActionDefinition: Add action for node `" << pNode->id << "'.";
      CActionQueue::addAction(mDelay, pAction);
    }
  catch (const std::exception & e)
    {
      CLogger::error() << "CActionDefinition: Failed to create action for node `" << pNode->id << "'.";
    }
}
