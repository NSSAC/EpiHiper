// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2025 Rector and Visitors of the University of Virginia 
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

#include <limits>
#include <jansson.h>

#include "actions/CActionDefinition.h"
#include "actions/CEdgeAction.h"
#include "actions/CNodeAction.h"
#include "actions/CVariableAction.h"
#include "actions/CActionQueue.h"
#include "actions/CCondition.h"
#include "actions/COperation.h"
#include "network/CNetwork.h"
#include "network/CEdge.h"
#include "network/CNode.h"
#include "utilities/CLogger.h"

// static 
std::map< double, std::set< CActionDefinition * > > CActionDefinition::Priorities;

// static 
void CActionDefinition::convertPrioritiesToOrder()
{
  size_t Order = 0;

  // This is the default priority used for progression and transmission actions
  Priorities.insert(std::make_pair(1.0, std::set< CActionDefinition * >()));

  std::map< double, std::set< CActionDefinition * > >::const_iterator it = Priorities.begin();
  std::map< double, std::set< CActionDefinition * > >::const_iterator end = Priorities.end();
  
  for (; it != end; ++it)
    {
      if (it->first == 1.0)
        CAction::setDefaultOrder(Order);

      std::set< CActionDefinition * >::const_iterator itSet = it->second.begin();
      std::set< CActionDefinition * >::const_iterator endSet = it->second.end();

      for (; itSet != endSet; ++itSet)
        (*itSet)->mOrder = Order;

      ++Order;
    }
}

// static 
size_t CActionDefinition::OrderSize()
{
  return Priorities.size();
}

// static
CActionDefinition * CActionDefinition::GetActionDefinition(const size_t & index)
{
  if (index < INSTANCES.size())
    return INSTANCES[index];

  return NULL;
}

// static 
void CActionDefinition::clear()
{
  INSTANCES.clear();
  Priorities.clear();
}

CActionDefinition::CActionDefinition()
  : CAnnotation()
  , mOperations()
  , mPriority(1.0)
  , mOrder(0)
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
  , mOrder(src.mOrder)
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
  , mOrder(0)
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
      Priorities[mPriority].insert(this);
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
          CLogger::error("Action: Invalid value for operations item '{}'.", i);
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
      ENABLE_TRACE(CLogger::trace("CActionDefinition::process: [ActionDefinition: {}] node and edge independent action.", mIndex););
      CActionQueue::addAction(mDelay, new CVariableAction(this));
    }
  catch (...)
    {
      CLogger::error("CActionDefinition::process: [ActionDefinition: {}] Failed to create node and edge independent action.", mIndex);
    }
}

void CActionDefinition::process(const CEdge * pEdge) const
{
  if (pEdge == NULL)
    return;

  try
    {
      ENABLE_TRACE(CLogger::trace("CActionDefinition::process: [ActionDefinition: {}] Add action for edge '{}, {}'.", mIndex, pEdge->targetId, pEdge->sourceId););
      CActionQueue::addAction(mDelay, new CEdgeAction(this, pEdge));
    }
  catch (...)
    {
      CLogger::error("CActionDefinition::process: [ActionDefinition: {}] Failed to create action for edge '{}, {}'.", mIndex, pEdge->targetId, pEdge->sourceId);
    }
}

void CActionDefinition::process(const CNode * pNode) const
{
  if (pNode == NULL)
    return;

  if (CNetwork::Context.Active().isRemoteNode(pNode))
    {
      ENABLE_TRACE(CLogger::error("CActionDefinition::process: [ActionDefinition: {}] Add remote action for node '{}'.", mIndex, (void *) pNode););
      // CActionQueue::addRemoteAction(mIndex, pNode);
      return;
    }

  try
    {
      ENABLE_TRACE(CLogger::trace("CActionDefinition::process: [ActionDefinition: {}] Add action for node '{}'.", mIndex, pNode->id););
      CActionQueue::addAction(mDelay, new CNodeAction(this, pNode));
    }
  catch (...)
    {
      CLogger::error("CActionDefinition::process: [ActionDefinition: {}] Failed to create action for node '{}'.", mIndex, pNode->id);
    }
}

bool CActionDefinition::execute() const
{
  bool success = true;

  try
    {
      if (mCondition.isTrue())
        {
          // Loop through the operation definitions
          std::vector< COperationDefinition >::const_iterator it = mOperations.begin();
          std::vector< COperationDefinition >::const_iterator end = mOperations.end();

          for (; it != end; ++it)
            it->execute(mInfo); 
        }
    }
  catch (...)
    {
      CLogger::error("CActionDefinition::execute: [ActionDefinition: {}] Failed to execute action.", mIndex);
      success = false;
    }

  return success;
}

bool CActionDefinition::execute(CEdge * pEdge) const
{
  bool success = true;

  try
    {
      if (mCondition.isTrue(pEdge))
        {
          // Loop through the operation definitions
          std::vector< COperationDefinition >::const_iterator it = mOperations.begin();
          std::vector< COperationDefinition >::const_iterator end = mOperations.end();

          for (; it != end; ++it)
            it->execute(pEdge, mInfo); 
        }
    }
  catch (...)
    {
      CLogger::error("CActionDefinition::execute: [ActionDefinition: {}] Failed to execute action for edge '{}, {}'.", mIndex, pEdge->targetId, pEdge->sourceId);
      success = false;
    }

  return success;
}

bool CActionDefinition::execute(CNode * pNode) const
{
  bool success = true;

  try
    {
      if (mCondition.isTrue(pNode))
        {
          // Loop through the operation definitions
          std::vector< COperationDefinition >::const_iterator it = mOperations.begin();
          std::vector< COperationDefinition >::const_iterator end = mOperations.end();

          for (; it != end; ++it)
            it->execute(pNode, mInfo); 
        }
    }
  catch (...)
    {
      CLogger::error("CActionDefinition::execute: [ActionDefinition: {}] Failed to execute action for node '{}'.", mIndex, pNode->id);
      success = false;
    }

  return success;
}

const size_t & CActionDefinition::getDelay() const
{
  return mDelay;
}
  

size_t CActionDefinition::getOrder() const
{
  return mOrder;
}
