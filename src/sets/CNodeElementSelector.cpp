// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2024 Rector and Visitors of the University of Virginia 
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

#include <algorithm>
#include <cstring>
#include <jansson.h>

#include "sets/CNodeElementSelector.h"
#include "sets/CSetCollector.h"
#include "math/CObservable.h"
#include "db/CConnection.h"
#include "db/CField.h"
#include "db/CFieldValue.h"
#include "db/CFieldValueList.h"
#include "db/CQuery.h"
#include "db/CSchema.h"
#include "network/CNetwork.h"
#include "network/CNode.h"
#include "network/CEdge.h"
#include "actions/CActionQueue.h"
#include "utilities/CLogger.h"
#include "utilities/CSimConfig.h"
#include "variables/CVariableList.h"

CNodeElementSelector::CNodeElementSelector()
  : CSetContent(CSetContent::Type::nodeElementSelector)
  , mNodeProperty()
  , mpValue(NULL)
  , mpValueList(NULL)
  , mpSelector(NULL)
  , mDBTable()
  , mDBField()
  , mpObservable(NULL)
  , mpVariable(NULL)
  , mpDBFieldValue(NULL)
  , mpDBFieldValueList(NULL)
  , mpComparison(NULL)
  , mSQLComparison("")
  , mLocalScope(true)
  , mpCompute(NULL)
  , mpFilter(NULL)
  , mpCollector(NULL)
{}

CNodeElementSelector::CNodeElementSelector(const CNodeElementSelector & src)
  : CSetContent(src)
  , mNodeProperty(src.mNodeProperty)
  , mpValue(src.mpValue != NULL ? new CValue(*src.mpValue) : NULL)
  , mpValueList(src.mpValueList != NULL ? new CValueList(*src.mpValueList) : NULL)
  , mpSelector(src.mpSelector)
  , mDBTable(src.mDBTable)
  , mDBField(src.mDBField)
  , mpObservable(src.mpObservable)
  , mpVariable(src.mpVariable)
  , mpDBFieldValue(src.mpDBFieldValue != NULL ? new CFieldValue(*src.mpDBFieldValue) : NULL)
  , mpDBFieldValueList(src.mpDBFieldValueList != NULL ? new CFieldValueList(*src.mpDBFieldValueList) : NULL)
  , mpComparison(src.mpComparison)
  , mSQLComparison(src.mSQLComparison)
  , mLocalScope(src.mLocalScope)
  , mpCompute(src.mpCompute)
  , mpFilter(src.mpFilter)
  , mpCollector(src.mpCollector)
{}

CNodeElementSelector::CNodeElementSelector(const json_t * json)
  : CSetContent(CSetContent::Type::nodeElementSelector)
  , mNodeProperty()
  , mpValue(NULL)
  , mpValueList(NULL)
  , mpSelector(NULL)
  , mDBTable()
  , mDBField()
  , mpObservable(NULL)
  , mpVariable(NULL)
  , mpDBFieldValue(NULL)
  , mpDBFieldValueList(NULL)
  , mpComparison(NULL)
  , mSQLComparison("")
  , mLocalScope(true)
  , mpCompute(NULL)
  , mpFilter(NULL)
  , mpCollector(NULL)
{
  fromJSON(json);
}

CNodeElementSelector::~CNodeElementSelector()
{
  if (mpValue != NULL)
    delete mpValue;

  if (mpValueList != NULL)
    delete mpValueList;

  if (mpDBFieldValue != NULL)
    delete mpDBFieldValue;

  if (mpDBFieldValueList != NULL)
    delete mpDBFieldValueList;

  if (mpCollector)
    mNodeProperty.deregisterSetCollector(mpCollector);
}

// virtual
void CNodeElementSelector::fromJSONProtected(const json_t * json)
{
  /*
  "nodeElementSelector": {
      "$id": "#nodeElementSelector",
      "description": "The specification of node elements of a set.",
      "type": "object",
      "allOf": [
        {
          "type": "object",
          "required": [
            "elementType",
            "scope"
          ],
          "properties": {
            "elementType": {
              "type": "string",
              "enum": ["node"]
            },
            "scope": {
              "type": "string",
              "enum": [
                "local",
                "global"
              ]
            }
          }
        },
        {
          "oneOf": [
            {
              "required": [
                "operator",
                "left",
                "right"
              ],
              "properties": {
                "operator": {"$ref": "#/definitions/comparisonOperator"},
                "left": {
                  "type": "object",
                  "required": ["node"],
                  "properties": {
                    "node": {"$ref": "#/definitions/nodeProperty"}
                  }
                },
                "right": {"$ref": "#/definitions/value"}
              }
            },
            {
              "description": "",
              "required": [
                "operator",
                "left",
                "right"
              ],
              "properties": {
                "operator": {
                  "description": "",
                  "type": "string",
                  "enum": [
                    "withPropertyIn",
                    "in",
                    "not in"
                  ]
                },
                "left": {
                  "type": "object",
                  "required": ["node"],
                  "properties": {
                    "node": {"$ref": "#/definitions/nodeProperty"}
                  }
                },
                "right": {
                  "oneOf": [
                    {
                      "$ref": "#/definitions/valueList"
                    },
                    {
                      "$ref": "#/definitions/setContent"
                    }
                  ]
                }
              }
            },
            {
              "description": "",
              "required": [
                "operator",
                "selector"
              ],
              "properties": {
                "operator": {
                  "description": "",
                  "type": "string",
                  "enum": ["withIncomingEdgeIn"]
                },
                "selector": {"$ref": "#/definitions/setContent"}
              }
            },
            {
              "description": "A filter selecting nodes from the external person trait database.",
              "oneOf": [
                {
                  "description": "A table in the external person trait database.",
                  "type": "object",
                  "required": ["table"],
                  "properties": {
                    "table": {"$ref": "#/definitions/uniqueIdRef"}
                  }
                },
                {
                  "description": "A filter returning nodes if the result of comparing left and right values with the operator is true.",
                  "required": [
                    "operator",
                    "left",
                    "right"
                  ],
                  "properties": {
                    "operator": {"$ref": "#/definitions/comparisonOperator"},
                    "left": {"$ref": "#/definitions/dbField"},
                    "right": {
                      "type": "object",
                      "description": "",
                      "oneOf": [
                        {"$ref": "#/definitions/dbFieldValue"},
                        {"$ref": "#/definitions/observable"}
                      ]
                    }
                  }
                },
                {
                  "description": "A filter returning nodes if the left field value is or is not in the right list.",
                  "required": [
                    "operator",
                    "left",
                    "right"
                  ],
                  "properties": {
                    "operator": {
                      "description": "",
                      "type": "string",
                      "enum": [
                        "in",
                        "not in"
                      ]
                    },
                    "left": {"$ref": "#/definitions/dbField"},
                    "right": {
                      "type": "object",
                      "description": "",
                      "oneOf": [
                        {"$ref": "#/definitions/dbFieldValueList"},
                        {"$ref": "#/definitions/dbFieldValueSelector"}
                      ]
                    }
                  }
                }
              ]
            }
          ]
        }
      ]
    }
  */

  mValid = false; // DONE
  mPrerequisites.clear();
  json_t * pValue = json_object_get(json, "elementType");

  if (json_is_string(pValue)
      && strcmp(json_string_value(pValue), "node") != 0)
    {
      CLogger::error("Node selector: Invalid or missing value 'elementType' for {}", CSimConfig::jsonToString(json));
      return;
    }

  // mPrerequisites.insert(&CActionQueue::getCurrentTick());
  pValue = json_object_get(json, "scope");

  if (pValue != NULL
      && strcmp(json_string_value(pValue), "local") == 0)
    {
      mLocalScope = true;
    }
  else if (pValue != NULL
           && strcmp(json_string_value(pValue), "global") == 0)
    {
      mLocalScope = false;
    }
  else
    {
      CLogger::error("Node selector: Invalid or missing value 'scope' for {}", CSimConfig::jsonToString(json));
      return;
    }

  pValue = json_object_get(json, "operator");

  if (pValue != NULL)
    {
      const char * Operator = json_string_value(pValue);

      if (strcmp(Operator, "==") == 0)
        {
          mpComparison = &operator==;
          mSQLComparison = "=";
        }
      else if (strcmp(Operator, "!=") == 0)
        {
          mpComparison = &operator!=;
          mSQLComparison = "<>";
        }
      else if (strcmp(Operator, "<=") == 0)
        {
          mpComparison = &operator<=;
          mSQLComparison = "<=";
        }
      else if (strcmp(Operator, "<") == 0)
        {
          mpComparison = &operator<;
          mSQLComparison = "<";
        }
      else if (strcmp(Operator, ">=") == 0)
        {
          mpComparison = &operator>=;
          mSQLComparison = ">=";
        }
      else if (strcmp(Operator, ">") == 0)
        {
          mpComparison = &operator>;
          mSQLComparison = ">";
        }
      else if (strcmp(Operator, "withPropertyIn") == 0
               || strcmp(Operator, "in") == 0
               || strcmp(Operator, "not in") == 0)
        {
          /*
          {
            "description": "",
            "required": [
              "operator",
              "left",
              "right"
            ],
            "properties": {
              "operator": {
                "description": "",
                "type": "string",
                   "enum": [
                     "withPropertyIn",
                     "in",
                     "not in"
                   ]
              },
              "left": {
                "type": "object",
                "required": ["node"],
                "properties": {
                  "node": {"$ref": "#/definitions/nodeProperty"}
                }
              },
              "right": {
                "oneOf": [
                  {
                    "$ref": "#/definitions/valueList"
                  },
                  {
                    "$ref": "#/definitions/setContent"
                  }
                ]
              }
            }
          },
          */

          mNodeProperty.fromJSON(json_object_get(json, "left"));

          if (mNodeProperty.isValid())
            {
              CLogger::pushLevel(CLogger::LogLevel::off);
              mpValueList = new CValueList(json_object_get(json, "right"));
              CLogger::popLevel();

              if (mpValueList != NULL)
                {
                  if (mpValueList->isValid())
                    {
                      if (strcmp(Operator, "not in") == 0)
                        {
                          mpCompute = &CNodeElementSelector::propertyNotIn;
                          mpFilter = &CNodeElementSelector::filterPropertyNotIn;
                        }
                      else
                        {
                          mpCompute = &CNodeElementSelector::propertyIn;
                          mpFilter = &CNodeElementSelector::filterPropertyIn;
                        }

                      if (!mNodeProperty.isReadOnly())
                        {
                          mPrerequisites.insert(&CActionQueue::getCurrentTick());

#ifdef USE_CSETCOLLECTOR
                          if (mLocalScope)
                            {
                              mpCollector = std::shared_ptr< CSetCollectorInterface >(new CSetCollector< CNode, CNodeElementSelector >(this));
                              mNodeProperty.registerSetCollector(mpCollector);
                            }
#endif
                        }

                      mValid = true;
                      return;
                    }
                  
                  if (mNodeProperty.getProperty() == CNodeProperty::Property::edges)
                    {
                      if (strcmp(Operator, "not in") == 0)
                        mpCompute = &CNodeElementSelector::withIncomingEdgeNotIn;
                      else
                        mpCompute = &CNodeElementSelector::withIncomingEdgeIn;
                    }
                  else
                    {
                      CLogger::error("Node selector: Invalid or missing value 'right' for {}", CSimConfig::jsonToString(json));
                      return;
                    }

                  mpSelector = CSetContent::create(json_object_get(json, "right"));
                  
                  if (mpSelector
                      && mpSelector->isValid())
                    {
                      mPrerequisites.insert(mpSelector.get()); // DONE
                      mValid = true;
                      return;
                    }

                  mpSelector.reset();

                  CLogger::error("Node selector: Invalid or missing value 'selector' for {}", CSimConfig::jsonToString(json));
                  return;
                }
            }

          if (strcmp(Operator, "in") == 0)
            {
              CConnection::setRequired(true);
              mpCompute = &CNodeElementSelector::dbIn;
            }
          else if (strcmp(Operator, "not in") == 0)
            {
              CConnection::setRequired(true);
              mpCompute = &CNodeElementSelector::dbNotIn;
            }
          else
            {
              CLogger::error("Node selector: Invalid or missing value 'left' for {}", CSimConfig::jsonToString(json));
              return;
            }
        }
      else if (strcmp(Operator, "withIncomingEdgeIn") == 0)
        {
          if (!mLocalScope)
            {
              CLogger::error("Node selector: Invalid or missing value 'scope' for {}", CSimConfig::jsonToString(json));
              return;
            }

          /*
            {
              "description": "",
              "required": [
                "operator",
                "selector"
              ],
              "properties": {
                "operator": {
                  "description": "",
                  "type": "string",
                  "enum": ["withIncomingEdgeIn"]
                },
                "selector": {"$ref": "#/definitions/setContent"}
              }
            },
              */
          // We need to identify that we have this case
          mpSelector = CSetContent::create(json_object_get(json, "selector"));

          if (mpSelector
              && mpSelector->isValid())
            {
              mPrerequisites.insert(mpSelector.get()); // DONE
              mpCompute = &CNodeElementSelector::withIncomingEdgeIn;
              mValid = true;
              return;
            }

          mpSelector.reset();

          CLogger::error("Node selector: Invalid or missing value 'selector' for {}", CSimConfig::jsonToString(json));
          return;
        }
    }
  else
    {
      // We do not have an operator, i.e., we have either all nodes or all nodes with a table.
      pValue = json_object_get(json, "table");

      if (json_is_string(pValue))
        {
          mDBTable = json_string_value(pValue);
        }

      if (CSchema::INSTANCE.getTable(mDBTable).isValid())
        {
          /*
            {
              "description": "A table in the external person trait database.",
              "type": "object",
              "required": ["table"],
              "properties": {
                "table": {"$ref": "#/definitions/uniqueIdRef"}
              }
            },
           */
          CConnection::setRequired(true);
          mpCompute = &CNodeElementSelector::dbAll;
          mValid = true;
          return;
        }
      else
        {
          if (!mLocalScope)
            {
              CLogger::error("Node selector: Invalid or missing value 'scope' for {}", CSimConfig::jsonToString(json));
              return;
            }

          mpCompute = &CNodeElementSelector::all;
          mValid = true;
          return;
        }

      CLogger::error("Node selector: Invalid or missing value 'operator' for {}", CSimConfig::jsonToString(json));
      return;
    }

  if (mpCompute == &CNodeElementSelector::dbIn
      || mpCompute == &CNodeElementSelector::dbNotIn)
    {
      /*
        {
          "description": "A filter selecting nodes from the external person trait database.",
          "oneOf": [
            {
              "description": "A filter returning nodes if the left field value is or is not in the right list.",
              "required": [
                "operator",
                "left",
                "right"
              ],
              "properties": {
                "operator": {
                  "description": "",
                  "type": "string",
                  "enum": [
                    "in",
                    "not in"
                  ]
                },
                "left": {"$ref": "#/definitions/dbField"},
                "right": {
                  "type": "object",
                  "description": "",
                  "oneOf": [
                    {"$ref": "#/definitions/dbFieldValueList"},
                    {"$ref": "#/definitions/dbFieldValueSelector"}
                  ]
                }
              }
            }
          ]
        }
       */

      json_t * pLeft = json_object_get(json, "left");

      pValue = json_object_get(pLeft, "table");

      if (json_is_string(pValue))
        {
          mDBTable = json_string_value(pValue);
        }

      pValue = json_object_get(pLeft, "field");

      if (json_is_string(pValue))
        {
          mDBField = json_string_value(pValue);

          if (mDBTable.empty())
            {
              mDBTable = CSchema::INSTANCE.getTableForField(mDBField);
            }
        }

      const CTable & Table = CSchema::INSTANCE.getTable(mDBTable);

      if (!Table.isValid())
        {
          CLogger::error("Node selector: Invalid or missing value 'table' for {}", CSimConfig::jsonToString(json));
          return;
        }

      const CField & Field = Table.getField(mDBField);

      if (!Field.isValid())
        {
          CLogger::error("Node selector: Invalid or missing value 'field' for {}", CSimConfig::jsonToString(json));
          return;
        }

      CFieldValueList FieldValueList(json_object_get(json, "right"), Field.getType());

      if (FieldValueList.isValid())
        {
          mpDBFieldValueList = new CFieldValueList(FieldValueList);
          mValid = true;
          return;
        }

      mpSelector = CSetContent::create(json_object_get(json, "right"));

      if (mpSelector
          && mpSelector->isValid())
        {
          mPrerequisites.insert(mpSelector.get()); // DONE
          mValid = true;
          return;
        }

      mpSelector.reset();

      CLogger::error("Node selector: Invalid or missing value 'right' for {}", CSimConfig::jsonToString(json));
      return;
    }

  // Select node where the node property value comparison with the provided value is true.
  if (mpComparison != NULL)
    {
      /*
        {
          "required": [
            "operator",
            "left",
            "right"
          ],
          "properties": {
            "operator": {"$ref": "#/definitions/comparisonOperator"},
            "left": {
              "type": "object",
              "required": ["node"],
              "properties": {
                "node": {"$ref": "#/definitions/nodeProperty"}
              }
            },
            "right": {"$ref": "#/definitions/value"}
          }
        }
       */

      mNodeProperty.fromJSON(json_object_get(json, "left"));

      if (mNodeProperty.isValid())
        {
          mpValue = new CValue(json_object_get(json, "right"));

          if (mpValue != NULL
              && mpValue->isValid())
            {
              mpCompute = &CNodeElementSelector::propertySelection;
              mpFilter = &CNodeElementSelector::filterPropertySelection;

              if (!mNodeProperty.isReadOnly())
                {
                  mPrerequisites.insert(&CActionQueue::getCurrentTick());

#ifdef USE_CSETCOLLECTOR
                  if (mLocalScope)
                    {
                      mpCollector = std::shared_ptr< CSetCollectorInterface >(new CSetCollector< CNode, CNodeElementSelector >(this));
                      mNodeProperty.registerSetCollector(mpCollector);
                    }
#endif
                }

              mValid = true;
              return;
            }

          CLogger::error("Node selector: Invalid or missing value 'right' for {}", CSimConfig::jsonToString(json));
          return;
        }

      /*
        {
          "description": "A filter returning nodes if the result of comparing left and right values with the operator is true.",
          "required": [
            "operator",
            "left",
            "right"
          ],
          "properties": {
            "operator": {"$ref": "#/definitions/comparisonOperator"},
            "left": {"$ref": "#/definitions/dbField"},
            "right": {
              "type": "object",
              "description": "",
              "oneOf": [
                {"$ref": "#/definitions/dbFieldValue"},
                {"$ref": "#/definitions/observable"}
              ]
            }
          }
        },
       */

      json_t * pLeft = json_object_get(json, "left");

      pValue = json_object_get(pLeft, "table");

      if (json_is_string(pValue))
        {
          mDBTable = json_string_value(pValue);
        }

      pValue = json_object_get(pLeft, "field");

      if (json_is_string(pValue))
        {
          mDBField = json_string_value(pValue);

          if (mDBTable.empty())
            {
              mDBTable = CSchema::INSTANCE.getTableForField(mDBField);
            }
        }

      const CTable & Table = CSchema::INSTANCE.getTable(mDBTable);

      if (!Table.isValid())
        {
          CLogger::error("Node selector: Invalid or missing value 'table' for {}", CSimConfig::jsonToString(json));
          return;
        }

      const CField & Field = Table.getField(mDBField);

      if (!Field.isValid())
        {
          CLogger::error("Node selector: Invalid or missing value 'field' for {}", CSimConfig::jsonToString(json));
          return;
        }

      CConnection::setRequired(true);
      mpCompute = &CNodeElementSelector::dbSelection;

      mpObservable = CObservable::get(json_object_get(json, "right"));

      if (mpObservable != NULL
          && mpObservable->isValid())
        {
          mPrerequisites.insert(mpObservable); // DONE
          mValid = true;
          return;
        }
      else
        {
          mpObservable = NULL;
        }

      mpVariable = &CVariableList::INSTANCE[json_object_get(json, "right")];

      if (mpVariable != NULL
          && mpVariable->isValid())
        {
          mPrerequisites.insert(mpVariable); // DONE
          mValid = true;
          return;
        }
      else
        {
          mpVariable = NULL;
        }

      CFieldValue FieldValue(json_object_get(json, "right"), Field.getType());

      if (FieldValue.isValid())
        {
          mpDBFieldValue = new CFieldValue(FieldValue);
          mValid = true;
          return;
        }
    }

  CLogger::error("Node selector: Invalid or missing value 'operator' for {}", CSimConfig::jsonToString(json));
}

// virtual 
bool CNodeElementSelector::lessThanProtected(const CSetContent & rhs) const
{
  const CNodeElementSelector * pRhs = static_cast< const CNodeElementSelector * >(&rhs);

  if (mNodeProperty != pRhs->mNodeProperty)  
    return mNodeProperty < pRhs->mNodeProperty;

  switch (comparePointer(mpValue, pRhs->mpValue))
  {
    case -1:
      return true;
      break;

    case 1:
      return false;
      break;
  }

  switch (comparePointer(mpValueList, pRhs->mpValueList))
  {
    case -1:
      return true;
      break;

    case 1:
      return false;
      break;
  }

  if (mpSelector != pRhs->mpSelector)  
    return mpSelector < pRhs->mpSelector;

  if (mDBTable != pRhs->mDBTable)  
    return mDBTable < pRhs->mDBTable;

  if (mDBField != pRhs->mDBField)  
    return mDBField < pRhs->mDBField;

  switch (comparePointer(mpObservable, pRhs->mpObservable))
  {
    case -1:
      return true;
      break;

    case 1:
      return false;
      break;
  }

  switch (comparePointer(mpVariable, pRhs->mpVariable))
  {
    case -1:
      return true;
      break;

    case 1:
      return false;
      break;
  }

  switch (comparePointer(mpDBFieldValue, pRhs->mpDBFieldValue))
  {
    case -1:
      return true;
      break;

    case 1:
      return false;
      break;
  }

  switch (comparePointer(mpDBFieldValueList, pRhs->mpDBFieldValueList))
  {
    case -1:
      return true;
      break;

    case 1:
      return false;
      break;
  }

  if (mSQLComparison != pRhs->mSQLComparison)  
    return mSQLComparison < pRhs->mSQLComparison;

  if (mLocalScope != pRhs->mLocalScope)  
    return mLocalScope < pRhs->mLocalScope;

  return pointerLessThan(mpCompute, pRhs->mpCompute);
}

// virtual 
void CNodeElementSelector::determineIsStatic()
{
  CComputable::determineIsStatic();

  if (mNodeProperty.isValid())
    mStatic &= mNodeProperty.isReadOnly();
}

// virtual
bool CNodeElementSelector::computeProtected()
{
  if (mValid)
    {
      if (mpCollector && mComputedOnce.Active())
        return mpCollector->apply();
      else if (mpCompute != NULL)
        {
          if (mpCollector)
            mpCollector->enable();

          return (this->*mpCompute)();
        }
    }

  return false;
}

// virtual 
CSetContent::FilterType CNodeElementSelector::filterType() const
{
  if (mpFilter != NULL
      && mpCollector == NULL)
    return FilterType::node;

  return FilterType::none;
}

// virtual
bool CNodeElementSelector::filter(const CNode * pNode) const
{
  return (this->*mpFilter)(pNode);
}

bool CNodeElementSelector::all()
{
  std::vector< CNode * > & Nodes = getNodes();

  if (Nodes.empty())
    {
      Nodes.resize(CNetwork::Context.Active().getLocalNodeCount());
      CNode * pNode = CNetwork::Context.Active().beginNode();
      std::vector< CNode * >::iterator it = Nodes.begin();
      std::vector< CNode * >::iterator end = Nodes.end();

      for (; it != end; ++it, ++pNode)
        *it = pNode;
    }

  CLogger::debug("CNodeElementSelector: all returned '{}' nodes.", Nodes.size());
  return true;
}

bool CNodeElementSelector::propertySelection()
{
  std::vector< CNode * > & Nodes = getNodes();
  Nodes.clear();

  CNode * pNode = CNetwork::Context.Active().beginNode();
  CNode * pNodeEnd = CNetwork::Context.Active().endNode();

  for (; pNode != pNodeEnd; ++pNode)
    if (filterPropertySelection(pNode))
      Nodes.push_back(pNode);

  CLogger::debug("CNodeElementSelector: propertySelection returned '{}' nodes.", Nodes.size());

  if (!mLocalScope)
    {
      CLogger::debug("CNodeElementSelector: Processing remote nodes");
      std::map< size_t, CNode * >::const_iterator it = CNetwork::Context.Active().beginRemoteNodes();
      std::map< size_t, CNode * >::const_iterator end = CNetwork::Context.Active().endRemoteNodes();
      bool sort = false;

      for (; it != end; ++it)
        if (filterPropertySelection(it->second))
          {
            Nodes.push_back(const_cast< CNode * >(it->second));
            sort = true;
          }

      if (sort)
        std::sort(Nodes.begin(), Nodes.end());

      CLogger::debug("CNodeElementSelector: propertySelection returned '{}' nodes.", Nodes.size());
    }

  return true;
}

bool CNodeElementSelector::filterPropertySelection(const CNode * pNode) const
{
  return mpComparison(mNodeProperty.propertyOf(pNode), *mpValue);
}

bool CNodeElementSelector::propertyIn()
{
  std::vector< CNode * > & Nodes = getNodes();
  Nodes.clear();

  CNode * pNode = CNetwork::Context.Active().beginNode();
  CNode * pNodeEnd = CNetwork::Context.Active().endNode();

  for (; pNode != pNodeEnd; ++pNode)
    if (mpValueList->contains(mNodeProperty.propertyOf(pNode)))
      Nodes.push_back(pNode);

  CLogger::debug("CNodeElementSelector: propertyIn returned '{}' nodes.", Nodes.size());

  if (!mLocalScope)
    {
      CLogger::debug("CNodeElementSelector: Processing remote nodes");

      std::map< size_t, CNode * >::const_iterator it = CNetwork::Context.Active().beginRemoteNodes();
      std::map< size_t, CNode * >::const_iterator end = CNetwork::Context.Active().endRemoteNodes();
      bool sort = false;

      for (; it != end; ++it)
        if (filterPropertyIn(it->second))
          {
            Nodes.push_back(const_cast< CNode * >(it->second));
            sort = true;
          }

      if (sort)
        std::sort(Nodes.begin(), Nodes.end());

      CLogger::debug("CNodeElementSelector: propertyIn returned '{}' nodes.", Nodes.size());
    }
  return true;
}

bool CNodeElementSelector::filterPropertyIn(const CNode * pNode) const
{
  return mpValueList->contains(mNodeProperty.propertyOf(pNode));
}

bool CNodeElementSelector::propertyNotIn()
{
  std::vector< CNode * > & Nodes = getNodes();
  Nodes.clear();

  CNode * pNode = CNetwork::Context.Active().beginNode();
  CNode * pNodeEnd = CNetwork::Context.Active().endNode();

  for (; pNode != pNodeEnd; ++pNode)
    if (!mpValueList->contains(mNodeProperty.propertyOf(pNode)))
      Nodes.push_back(pNode);

  CLogger::debug("CNodeElementSelector: propertyNotIn returned '{}' nodes.", Nodes.size());

  if (!mLocalScope)
    {
      CLogger::debug("CNodeElementSelector: Processing remote nodes");

      std::map< size_t, CNode * >::const_iterator it = CNetwork::Context.Active().beginRemoteNodes();
      std::map< size_t, CNode * >::const_iterator end = CNetwork::Context.Active().endRemoteNodes();
      bool sort = false;

      for (; it != end; ++it)
        if (filterPropertyNotIn(it->second))
          {
            Nodes.push_back(const_cast< CNode * >(it->second));
            sort = true;
          }

      if (sort)
        std::sort(Nodes.begin(), Nodes.end());

      CLogger::debug("CNodeElementSelector: propertyNotIn returned '{}' nodes.", Nodes.size());
    }

  return true;
}

bool CNodeElementSelector::filterPropertyNotIn(const CNode * pNode) const
{
  return !mpValueList->contains(mNodeProperty.propertyOf(pNode));
}

bool CNodeElementSelector::withIncomingEdgeIn()
{
  std::vector< CNode * > & Nodes = getNodes();
  Nodes.clear();

  CNode * pNode = NULL;

  const std::vector< CEdge * > Edges = mpSelector->getEdges();
  CLogger::debug("CNodeElementSelector: withIncomingEdgeNotIn edges {}", Edges.size());

  if (!Edges.empty())
    {
      std::vector< CEdge * >::const_iterator it = Edges.begin();
      std::vector< CEdge * >::const_iterator end = Edges.end();

      for (; it != end; ++it)
        if ((*it)->pTarget != pNode)
          {
            pNode = (*it)->pTarget;
            Nodes.push_back(pNode);
          }

      // The edges are sorted by target node. Therfore, we do not need to sort
    }

  CLogger::debug("CNodeElementSelector: withIncomingEdgeIn returned '{}' nodes.", Nodes.size());
  return true;
}

bool CNodeElementSelector::withIncomingEdgeNotIn()
{
  std::vector< CNode * > & Nodes = getNodes();
  Nodes.clear();

  const std::vector< CEdge * > Edges = mpSelector->getEdges();
  CLogger::debug("CNodeElementSelector: withIncomingEdgeNotIn edges {}", Edges.size());
  
  if (!Edges.empty())
    {
      CNetwork & Active = CNetwork::Context.Active();

      CNode * pNode = Active.beginNode();
      CNode * pNodeEnd = Active.endNode();
      std::vector< CEdge * >::const_iterator itEdge = Edges.begin();
      std::vector< CEdge * >::const_iterator endEdge = Edges.end();

      while (pNode < pNodeEnd && itEdge != endEdge)
        {
          if (pNode < (*itEdge)->pTarget)
            {
              Nodes.push_back(pNode);
              ++pNode;
            }
          else if (pNode > (*itEdge)->pTarget)
            {
              ++itEdge;
            }
          else
            {
              ++pNode;
            }
        }

      while (pNode < pNodeEnd)
        {
          Nodes.push_back(pNode);
          ++pNode;
        }

      // Since the nodes are sorted we have no need to sort
    }

  CLogger::debug("CNodeElementSelector: withIncomingEdgeNotIn returned '{}' nodes.", Nodes.size());
  return true;
}

bool CNodeElementSelector::dbAll()
{
  std::vector< CNode * > & Nodes = getNodes();
  Nodes.clear();

  CFieldValueList FieldValueList;
  bool success = CQuery::all(mDBTable, "pid", FieldValueList, mLocalScope);

  CFieldValueList::const_iterator it = FieldValueList.begin();
  CFieldValueList::const_iterator end = FieldValueList.end();
  CNode * pNode;

  for (; it != end; ++it)
    {
      if ((pNode = CNetwork::Context.Active().lookupNode(it->toId(), mLocalScope)) != NULL)
        Nodes.push_back(pNode);
    }

  if (!mLocalScope)
    std::sort(Nodes.begin(), Nodes.end());

  CLogger::debug("CNodeElementSelector: dbAll returned '{}' nodes.", Nodes.size());
  return success;
}

bool CNodeElementSelector::dbSelection()
{
  bool success = false;
  std::vector< CNode * > & Nodes = getNodes();
  Nodes.clear();

  CFieldValueList FieldValueList;

  if (mpObservable)
    success = CQuery::where(mDBTable, "pid", FieldValueList, mLocalScope, mDBField, *mpObservable, mSQLComparison);
  else if (mpVariable)
    success = CQuery::where(mDBTable, "pid", FieldValueList, mLocalScope, mDBField, mpVariable->toValue(), mSQLComparison);
  else
    success = CQuery::where(mDBTable, "pid", FieldValueList, mLocalScope, mDBField, *mpDBFieldValue, mSQLComparison);

  CFieldValueList::const_iterator it = FieldValueList.begin();
  CFieldValueList::const_iterator end = FieldValueList.end();
  CNode * pNode;

  for (; it != end; ++it)
    {
      if ((pNode = CNetwork::Context.Active().lookupNode(it->toId(), mLocalScope)) != NULL)
        Nodes.push_back(pNode);
    }

  if (!mLocalScope)
    std::sort(Nodes.begin(), Nodes.end());

  CLogger::debug("CNodeElementSelector: dbSelection returned '{}' nodes.", Nodes.size());
  return success;
}

bool CNodeElementSelector::dbIn()
{
  bool success = false;
  std::vector< CNode * > & Nodes = getNodes();
  Nodes.clear();

  CFieldValueList FieldValueList;

  if (mpDBFieldValueList != NULL)
    success = CQuery::in(mDBTable, "pid", FieldValueList, mLocalScope, mDBField, *mpDBFieldValueList);
  else
    {
      CField Field = CSchema::INSTANCE.getTable(mDBTable).getField(mDBField);
      const CDBFieldValues & ValueListMap = mpSelector->getDBFieldValues();
      CDBFieldValues::const_iterator found = ValueListMap.find(Field.getType());

      if (found != ValueListMap.end())
        success = CQuery::in(mDBTable, "pid", FieldValueList, mLocalScope, mDBField, found->second);
    }

  CFieldValueList::const_iterator it = FieldValueList.begin();
  CFieldValueList::const_iterator end = FieldValueList.end();
  CNode * pNode;

  for (; it != end; ++it)
    {
      if ((pNode = CNetwork::Context.Active().lookupNode(it->toId(), mLocalScope)) != NULL)
        Nodes.push_back(pNode);
    }

  if (!mLocalScope)
    std::sort(Nodes.begin(), Nodes.end());

  CLogger::debug("CNodeElementSelector: dbIn returned '{}' nodes.", Nodes.size());
  return success;
}

bool CNodeElementSelector::dbNotIn()
{
  bool success = false;
  std::vector< CNode * > & Nodes = getNodes();
  Nodes.clear();

  CFieldValueList FieldValueList;

  if (mpDBFieldValueList != NULL)
    success = CQuery::notIn(mDBTable, "pid", FieldValueList, mLocalScope, mDBField, *mpDBFieldValueList);
  else
    {
      CField Field = CSchema::INSTANCE.getTable(mDBTable).getField(mDBField);
      const CDBFieldValues & ValueListMap = mpSelector->getDBFieldValues();
      CDBFieldValues::const_iterator found = ValueListMap.find(Field.getType());

      if (found != ValueListMap.end())
        success = CQuery::notIn(mDBTable, "pid", FieldValueList, mLocalScope, mDBField, found->second);
    }

  CFieldValueList::const_iterator it = FieldValueList.begin();
  CFieldValueList::const_iterator end = FieldValueList.end();
  CNode * pNode;

  for (; it != end; ++it)
    {
      if ((pNode = CNetwork::Context.Active().lookupNode(it->toId(), mLocalScope)) != NULL)
        Nodes.push_back(pNode);
    }

  if (!mLocalScope)
    std::sort(Nodes.begin(), Nodes.end());

  CLogger::debug("CNodeElementSelector: dbNotIn returned '{}' nodes.", Nodes.size());
  return success;
}
