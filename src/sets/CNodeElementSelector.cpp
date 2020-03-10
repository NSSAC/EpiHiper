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

#include <algorithm>
#include <cstring>
#include <jansson.h>

#include "sets/CNodeElementSelector.h"
#include "math/CObservable.h"
#include "db/CConnection.h"
#include "db/CFieldValue.h"
#include "db/CFieldValueList.h"
#include "db/CQuery.h"
#include "db/CSchema.h"
#include "network/CNetwork.h"
#include "network/CNode.h"
#include "network/CEdge.h"
#include "actions/CActionQueue.h"
#include "utilities/CLogger.h"

CNodeElementSelector::CNodeElementSelector()
  : CSetContent()
  , mNodeProperty()
  , mpValue(NULL)
  , mpValueList(NULL)
  , mpSelector(NULL)
  , mDBTable()
  , mDBField()
  , mpObservable(NULL)
  , mpDBFieldValue(NULL)
  , mpDBFieldValueList(NULL)
  , mpComparison(NULL)
  , mSQLComparison("")
  , mLocalScope(true)
  , mpCompute(NULL)
{}

CNodeElementSelector::CNodeElementSelector(const CNodeElementSelector & src)
  : CSetContent(src)
  , mNodeProperty(src.mNodeProperty)
  , mpValue(src.mpValue != NULL ? new CValue(*src.mpValue) : NULL)
  , mpValueList(src.mpValueList != NULL ? new CValueList(*src.mpValueList) : NULL)
  , mpSelector(src.mpSelector != NULL ? src.mpSelector->copy() : NULL)
  , mDBTable(src.mDBTable)
  , mDBField(src.mDBField)
  , mpObservable(src.mpObservable != NULL ? new CObservable(*src.mpObservable) : NULL)
  , mpDBFieldValue(src.mpDBFieldValue != NULL ? new CFieldValue(*src.mpDBFieldValue) : NULL)
  , mpDBFieldValueList(src.mpDBFieldValueList != NULL ? new CFieldValueList(*src.mpDBFieldValueList) : NULL)
  , mpComparison(src.mpComparison)
  , mSQLComparison(src.mSQLComparison)
  , mLocalScope(src.mLocalScope)
  , mpCompute(src.mpCompute)
{}

CNodeElementSelector::CNodeElementSelector(const json_t * json)
  : CSetContent()
  , mNodeProperty()
  , mpValue(NULL)
  , mpValueList(NULL)
  , mpSelector(NULL)
  , mDBTable()
  , mDBField()
  , mpObservable(NULL)
  , mpDBFieldValue(NULL)
  , mpDBFieldValueList(NULL)
  , mpComparison(NULL)
  , mSQLComparison("")
  , mLocalScope(true)
  , mpCompute(NULL)
{
  fromJSON(json);
}

CNodeElementSelector::~CNodeElementSelector()
{
  if (mpValue != NULL)
    delete mpValue;
  if (mpValueList != NULL)
    delete mpValueList;
  CSetContent::destroy(mpSelector);
  if (mpDBFieldValue != NULL)
    delete mpDBFieldValue;
  if (mpDBFieldValueList != NULL)
    delete mpDBFieldValueList;
}

// virtual
CSetContent * CNodeElementSelector::copy() const
{
  return new CNodeElementSelector(*this);
}

// virtual
void CNodeElementSelector::fromJSON(const json_t * json)
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
                  "enum": ["withPropertyIn"]
                },
                "left": {
                  "type": "object",
                  "required": ["node"],
                  "properties": {
                    "node": {"$ref": "#/definitions/nodeProperty"}
                  }
                },
                "right": {"$ref": "#/definitions/valueList"}
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
                  "description": "A filter returining nodes if the result of comparing left and right values with the operator is true.",
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
      CLogger::error("Node selector: Invalid value for 'elementType'.");
      return;
    }

  mPrerequisites.insert(&CActionQueue::getCurrentTick());
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
      CLogger::error("Node selector: Invalid or missing value for 'scope'.");
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
      else if (strcmp(Operator, "withPropertyIn") == 0)
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
               "enum": ["withPropertyIn"]
             },
             "left": {
               "type": "object",
               "required": ["node"],
               "properties": {
                 "node": {"$ref": "#/definitions/nodeProperty"}
               }
             },
             "right": {"$ref": "#/definitions/valueList"}
           }
         },
           */

          mNodeProperty.fromJSON(json_object_get(json, "left"));

          if (!mNodeProperty.isValid())
            {
              CLogger::error("Node selector: Invalid or missing value for 'left'.");
              return;
            }

          mpValueList = new CValueList(json_object_get(json, "right"));

          if (mpValueList != NULL
              && !mpValueList->isValid())
            {
              CLogger::error("Node selector: Invalid or missing value for 'right'.");
              return;
            }

          mpCompute = &CNodeElementSelector::nodePropertyWithin;
          mValid = true;
          return;
        }
      else if (strcmp(Operator, "withIncomingEdgeIn") == 0)
        {
          if (!mLocalScope)
            {
              CLogger::error("Node selector: Invalid  value for 'scope'.");
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

          if (mpSelector != NULL
              && mpSelector->isValid())
            {
              mPrerequisites.insert(mpSelector);
              mStatic = mpSelector->isStatic();
              mpCompute = &CNodeElementSelector::nodeWithIncomingEdge;
              mValid = true;
              return;
            }

          if (mpSelector != NULL)
            {
              delete mpSelector;
              mpSelector = NULL;
            }

          CLogger::error("Node selector: Invalid or missing value for 'selector'.");
          return;
        }
      else if (strcmp(Operator, "in") == 0)
        {
          CConnection::setRequired(true);
          mpCompute = &CNodeElementSelector::nodeWithDBFieldWithin;
        }
      else if (strcmp(Operator, "not in") == 0)
        {
          CConnection::setRequired(true);
          mpCompute = &CNodeElementSelector::nodeWithDBFieldNotWithin;
        }
    }
  else
    {
      // We do not have an operator, i.e., we have either all nodes or all nodes with a table.
      mStatic = true;
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
          mpCompute = &CNodeElementSelector::nodeInDBTable;
          mValid = true;
          return;
        }
      else
        {
          if (!mLocalScope)
            {
              CLogger::error("Node selector: Invalid  value for 'scope'.");
              return;
            }

          mpCompute = &CNodeElementSelector::nodeAll;
          mValid = true;
          return;
        }

      CLogger::error("Node selector: Missing value for 'operator'.");
      return;
    }

  if (mpCompute == &CNodeElementSelector::nodeWithDBFieldWithin
      || mpCompute == &CNodeElementSelector::nodeWithDBFieldNotWithin)
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

      // We do not have an operator, i.e., we have either all nodes or all nodes with a table.
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
          CLogger::error("Node selector: Invalid or missing value for 'mDBTable'.");
          return;
        }

      if (!Table.getField(mDBField).isValid())
        {
          CLogger::error("Node selector: Invalid or missing value for 'mDBField'.");
          return;
        }

      CFieldValueList FieldValueList(json_object_get(json, "right"));

      if (FieldValueList.isValid())
        {
          mpDBFieldValueList = new CFieldValueList(FieldValueList);
          mStatic = true;
          mValid = true;
          return;
        }

      mpSelector = CSetContent::create(json_object_get(json, "right"));

      if (mpSelector != NULL
          && mpSelector->isValid())
        {
          mPrerequisites.insert(mpSelector);
          mStatic = mpSelector->isStatic();
          mValid = true;
          return;
        }

      if (mpSelector != NULL)
        {
          delete mpSelector;
          mpSelector = NULL;
        }

      CLogger::error("Node selector: Invalid or missing value for 'right'.");
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
              mpCompute = &CNodeElementSelector::nodePropertySelection;
              mValid = true;
              return;
            }

          CLogger::error("Node selector: Invalid value for 'right'.");
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

      // We do not have an operator, i.e., we have either all nodes or all nodes with a table.
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
          CLogger::error("Node selector: Invalid or missing value for 'mDBTable'.");
          return;
        }

      if (!Table.getField(mDBField).isValid())
        {
          CLogger::error("Node selector: Invalid or missing value for 'mDBField'.");
          return;
        }

      CConnection::setRequired(true);
      mpCompute = &CNodeElementSelector::nodeWithDBFieldSelection;

      mpObservable = CObservable::get(json_object_get(json, "right"));

      if (mpObservable != NULL
          && mpObservable->isValid())
        {
          mPrerequisites.insert(mpObservable);
          mValid = true;
          return;
        }

      CFieldValue FieldValue(json_object_get(json, "right"));

      if (FieldValue.isValid())
        {
          mpDBFieldValue = new CFieldValue(FieldValue);
          mStatic = true;
          mValid = true;
          return;
        }
    }

  CLogger::error("Node selector: Invalid or missing value for 'operator'.");
}

// virtual
bool CNodeElementSelector::computeProtected()
{
  if (mValid
      && mpCompute != NULL)
    return (this->*mpCompute)();

  return false;
}

bool CNodeElementSelector::nodeAll()
{
  std::vector< CNode * > & Nodes = getNodes();

  if (Nodes.empty())
    {
      Nodes.resize(CNetwork::INSTANCE->getLocalNodeCount());
      CNode * pNode = CNetwork::INSTANCE->beginNode();
      std::vector< CNode * >::iterator it = Nodes.begin();
      std::vector< CNode * >::iterator end = Nodes.end();

      for (; it != end; ++it, ++pNode)
        *it = pNode;
    }

  return true;
}

bool CNodeElementSelector::nodePropertySelection()
{
  std::vector< CNode * > & Nodes = getNodes();
  Nodes.clear();

  CNode * pNode = CNetwork::INSTANCE->beginNode();
  CNode * pNodeEnd = CNetwork::INSTANCE->endNode();

  for (; pNode != pNodeEnd; ++pNode)
    if (mpComparison(mNodeProperty.propertyOf(pNode), *mpValue))
      Nodes.push_back(pNode);

  if (mLocalScope)
    return true;

  std::map< size_t, CNode >::const_iterator it = CNetwork::INSTANCE->beginRemoteNodes();
  std::map< size_t, CNode >::const_iterator end = CNetwork::INSTANCE->endRemoteNodes();

  for (; it != end; ++it)
    if (mpComparison(mNodeProperty.propertyOf(&it->second), *mpValue))
      Nodes.push_back(const_cast< CNode * >(&it->second));

  return true;
}

bool CNodeElementSelector::nodePropertyWithin()
{
  std::vector< CNode * > & Nodes = getNodes();
  Nodes.clear();

  CNode * pNode = CNetwork::INSTANCE->beginNode();
  CNode * pNodeEnd = CNetwork::INSTANCE->endNode();

  for (; pNode != pNodeEnd; ++pNode)
    if (mpValueList->contains(mNodeProperty.propertyOf(pNode)))
      Nodes.push_back(pNode);

  if (mLocalScope)
    return true;

  std::map< size_t, CNode >::const_iterator it = CNetwork::INSTANCE->beginRemoteNodes();
  std::map< size_t, CNode >::const_iterator end = CNetwork::INSTANCE->endRemoteNodes();

  for (; it != end; ++it)
    if (mpValueList->contains(mNodeProperty.propertyOf(&it->second)))
      Nodes.push_back(const_cast< CNode * >(&it->second));

  return true;
}

bool CNodeElementSelector::nodeWithIncomingEdge()
{
  std::vector< CNode * > & Nodes = getNodes();
  Nodes.clear();

  CNode * pNode = NULL;
  std::vector< CEdge * >::const_iterator it = mpSelector->beginEdges();
  std::vector< CEdge * >::const_iterator end = mpSelector->endEdges();

  for (; it != end; ++it)
    if ((*it)->pTarget != pNode)
      {
        pNode = (*it)->pTarget;
        Nodes.push_back(pNode);
      }

  std::sort(Nodes.begin(), Nodes.end());
  // std::cout << "nodeWithIncomingEdge: " << Nodes.size() << std::endl;

  return true;
}

bool CNodeElementSelector::nodeInDBTable()
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
      if ((pNode = CNetwork::INSTANCE->lookupNode(it->toId(), mLocalScope)) != NULL)
        Nodes.push_back(pNode);
    }

  // std::cout << "nodeInDBTable: " << Nodes.size() << std::endl;
  return success;
}

bool CNodeElementSelector::nodeWithDBFieldSelection()
{
  bool success = false;
  std::vector< CNode * > & Nodes = getNodes();
  Nodes.clear();

  CFieldValueList FieldValueList;

  if (mpObservable)
    success = CQuery::where(mDBTable, "pid", FieldValueList, mLocalScope, mDBField, *mpObservable, mSQLComparison);
  else
    success = CQuery::where(mDBTable, "pid", FieldValueList, mLocalScope, mDBField, *mpDBFieldValue, mSQLComparison);

  CFieldValueList::const_iterator it = FieldValueList.begin();
  CFieldValueList::const_iterator end = FieldValueList.end();
  CNode * pNode;

  for (; it != end; ++it)
    {
      if ((pNode = CNetwork::INSTANCE->lookupNode(it->toId(), mLocalScope)) != NULL)
        Nodes.push_back(pNode);
    }
  // std::cout << "nodeWithDBFieldSelection: " << Nodes.size() << std::endl;
  return success;
}

bool CNodeElementSelector::nodeWithDBFieldWithin()
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
      const std::map< CValueList::Type, CValueList > & ValueListMap = mpSelector->getDBFieldValues();
      std::map< CValueList::Type, CValueList >::const_iterator found = ValueListMap.find(Field.getType());

      if (found != ValueListMap.end())
        success = CQuery::in(mDBTable, "pid", FieldValueList, mLocalScope, mDBField, found->second);
    }

  CFieldValueList::const_iterator it = FieldValueList.begin();
  CFieldValueList::const_iterator end = FieldValueList.end();
  CNode * pNode;

  for (; it != end; ++it)
    {
      if ((pNode = CNetwork::INSTANCE->lookupNode(it->toId(), mLocalScope)) != NULL)
        Nodes.push_back(pNode);
    }
  // std::cout << "nodeWithDBFieldWithin: " << Nodes.size() << std::endl;
  return success;
}

bool CNodeElementSelector::nodeWithDBFieldNotWithin()
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
      const std::map< CValueList::Type, CValueList > & ValueListMap = mpSelector->getDBFieldValues();
      std::map< CValueList::Type, CValueList >::const_iterator found = ValueListMap.find(Field.getType());

      if (found != ValueListMap.end())
        success = CQuery::notIn(mDBTable, "pid", FieldValueList, mLocalScope, mDBField, found->second);
    }

  CFieldValueList::const_iterator it = FieldValueList.begin();
  CFieldValueList::const_iterator end = FieldValueList.end();
  CNode * pNode;

  for (; it != end; ++it)
    {
      if ((pNode = CNetwork::INSTANCE->lookupNode(it->toId(), mLocalScope)) != NULL)
        Nodes.push_back(pNode);
    }
  // std::cout << "nodeWithDBFieldNotWithin: " << Nodes.size() << std::endl;
  return success;
}
