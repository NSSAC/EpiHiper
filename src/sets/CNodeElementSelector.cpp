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

#include <cstring>
#include <jansson.h>

#include "sets/CNodeElementSelector.h"
#include "math/CObservable.h"
#include "db/CFieldValue.h"
#include "db/CFieldValueList.h"
#include "db/CQuery.h"
#include "db/CSchema.h"
#include "network/CNetwork.h"
#include "network/CNode.h"
#include "network/CEdge.h"

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
  , mpSelector(CSetContent::copy(src.mpSelector))
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
  if (mpValue == NULL) delete mpValue;
  if (mpValueList == NULL) delete mpValueList;
  CSetContent::destroy(mpSelector);
  if (mpDBFieldValue == NULL) delete mpDBFieldValue;
  if (mpDBFieldValueList == NULL) delete mpDBFieldValueList;
}

// virtual
void CNodeElementSelector::fromJSON(const json_t * json)
{
  json_t * pValue = json_object_get(json, "elementType");

  mValid = (json_is_string(pValue) && strcmp(json_string_value(pValue), "node") == 0);
  mPrerequisites.clear();

  if (!mValid) return;

  pValue = json_object_get(json, "scope");

  if (strcmp(json_string_value(pValue), "local") == 0)
    {
      mLocalScope = true;
    }
  else if(strcmp(json_string_value(pValue), "global") == 0)
    {
      mLocalScope = false;
    }
  else
    {
      mValid = false;
    }

  pValue = json_object_get(json, "operator");

  if (strcmp(json_string_value(pValue), "==") == 0)
    {
      mpComparison = &operator==;
      mSQLComparison = "=";
    }
  else if (strcmp(json_string_value(pValue), "!=") == 0)
    {
      mpComparison = &operator!=;
      mSQLComparison = "<>";
    }
  else if (strcmp(json_string_value(pValue), "<=") == 0)
    {
      mpComparison = &operator<=;
      mSQLComparison = "<=";
    }
  else if (strcmp(json_string_value(pValue), "<") == 0)
    {
      mpComparison = &operator<;
      mSQLComparison = "<";
    }
  else if (strcmp(json_string_value(pValue), ">=") == 0)
    {
      mpComparison = &operator>=;
      mSQLComparison = ">=";
    }
  else if (strcmp(json_string_value(pValue), ">") == 0)
    {
      mpComparison = &operator>;
      mSQLComparison = ">";
    }
  else if (strcmp(json_string_value(pValue), "withPropertyIn") == 0)
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
       mValid &= mNodeProperty.isValid();
       mpValueList = new CValueList(json_object_get(json, "right"));
       mValid &= (mpValueList != NULL && mpValueList->isValid());
       mpCompute = &CNodeElementSelector::nodePropertyWithin;

       return;
    }
  else if (strcmp(json_string_value(pValue), "withIncomingEdgeIn") == 0)
    {
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
      mValid &= (mpSelector != NULL && mpSelector->isValid());

      if (mValid)
        mPrerequisites.insert(mpSelector);

      mpCompute = &CNodeElementSelector::nodeWithIncomingEdge;

      return;
    }
  else if (strcmp(json_string_value(pValue), "in") == 0)
    {
      mpCompute = &CNodeElementSelector::nodeWithDBFieldWithin;
    }
  else if (strcmp(json_string_value(pValue), "not in") == 0)
    {
      mpCompute = &CNodeElementSelector::nodeWithDBFieldNotWithin;
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
          mpCompute = &CNodeElementSelector::nodeInDBTable;
          mValid = true;
        }
      else
        {
          mpCompute = &CNodeElementSelector::nodeAll;

          // It is only possible to
          mValid = mLocalScope;
        }

      return;
    }

  if (mpCompute == &CNodeElementSelector::nodeWithDBFieldWithin ||
      mpCompute == &CNodeElementSelector::nodeWithDBFieldNotWithin)
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
      pValue = json_object_get(json, "table");

      if (json_is_string(pValue))
        {
          mDBTable = json_string_value(pValue);
        }

      const CTable & Table = CSchema::INSTANCE.getTable(mDBTable);

      if (!Table.isValid())
        {
          mValid = false;
          return;
        }

      pValue = json_object_get(json, "field");

      if (json_is_string(pValue))
        {
          mDBField = json_string_value(pValue);
        }

      if (!Table.getField(mDBField).isValid())
        {
          mValid = false;
          return;
        }

      mValid = true;

      CFieldValueList FieldValueList(json_object_get(json, "right"));

      if (FieldValueList.isValid())
        {
          mpDBFieldValueList = new CFieldValueList(FieldValueList);
          mValid = true;

          return;
        }

      mpSelector = CSetContent::create(json_object_get(json, "right"));
      mPrerequisites.insert(mpSelector);

      mValid = (mpSelector != NULL && mpSelector->isValid());

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
          mValid = mLocalScope;
          mpValue = new CValue(json_object_get(json, "right"));
          mValid &= (mpValue != NULL && mpValue->isValid());
          mpCompute = &CNodeElementSelector::nodePropertySelection;

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
      pValue = json_object_get(json, "table");

      if (json_is_string(pValue))
        {
          mDBTable = json_string_value(pValue);
        }

      const CTable & Table = CSchema::INSTANCE.getTable(mDBTable);

      if (!Table.isValid())
        {
          mValid = false;
          return;
        }

      pValue = json_object_get(json, "field");

      if (json_is_string(pValue))
        {
          mDBField = json_string_value(pValue);
        }

      if (!Table.getField(mDBField).isValid())
        {
          mValid = false;
          return;
        }

      mpCompute = &CNodeElementSelector::nodeWithDBFieldSelection;

      mpObservable = CObservable::get(json_object_get(json, "right"));

      if (mpObservable->isValid())
        {
          mPrerequisites.insert(mpObservable);
          mValid = true;

          return;
        }

      CFieldValue FieldValue(json_object_get(json, "right"));

      if (FieldValue.isValid())
        {
          mpDBFieldValue = new CFieldValue(FieldValue);
          mValid = true;

          return;
        }
    }

  mValid = false;
  return;
}

// virtual
void CNodeElementSelector::compute()
{
  if (mValid &&
      mpCompute != NULL)
    (this->*mpCompute)();
}

void CNodeElementSelector::nodeAll()
{
  if (mNodes.empty())
    {
      CNode * pNode = CNetwork::INSTANCE->beginNode();
      CNode * pNodeEnd = CNetwork::INSTANCE->endNode();

      for (; pNode != pNodeEnd; ++pNode)
        mNodes.insert(pNode);
    }
}

void CNodeElementSelector::nodePropertySelection()
{
  mNodes.clear();

  CNode * pNode = CNetwork::INSTANCE->beginNode();
  CNode * pNodeEnd = CNetwork::INSTANCE->endNode();

  for (; pNode != pNodeEnd; ++pNode)
    if (mpComparison(mNodeProperty.propertyOf(pNode), *mpValue))
      mNodes.insert(pNode);
}

void CNodeElementSelector::nodePropertyWithin()
{
  mNodes.clear();

  CNode * pNode = CNetwork::INSTANCE->beginNode();
  CNode * pNodeEnd = CNetwork::INSTANCE->endNode();

  for (; pNode != pNodeEnd; ++pNode)
    if (mpValueList->contains(mNodeProperty.propertyOf(pNode)))
      mNodes.insert(pNode);
}

void CNodeElementSelector::nodeWithIncomingEdge()
{
  mNodes.clear();

  CNode * pNode = NULL;
  std::set< CEdge * >::const_iterator it = mpSelector->beginEdges();
  std::set< CEdge * >::const_iterator end = mpSelector->endEdges();

  for (; it != end; ++it)
    if ((*it)->pTarget != pNode)
      {
        pNode = (*it)->pTarget;
        mNodes.insert(pNode);
      }
}

void CNodeElementSelector::nodeInDBTable()
{
  mNodes.clear();

  CFieldValueList FieldValueList;
  CQuery::all(mDBTable, "pid", FieldValueList, mLocalScope);

  CFieldValueList::const_iterator it = FieldValueList.begin();
  CFieldValueList::const_iterator end = FieldValueList.end();

  for (; it != end; ++it)
    {
      mNodes.insert(CNetwork::INSTANCE->lookupNode(it->toId(), mLocalScope));
    }
}

void CNodeElementSelector::nodeWithDBFieldSelection()
{
  mNodes.clear();
  CFieldValueList FieldValueList;

  if (mpObservable)
    CQuery::where(mDBTable, "pid", FieldValueList, mLocalScope, mDBField, *mpObservable, mSQLComparison);
  else
    CQuery::where(mDBTable, "pid", FieldValueList, mLocalScope, mDBField, *mpDBFieldValue, mSQLComparison);

  CFieldValueList::const_iterator it = FieldValueList.begin();
  CFieldValueList::const_iterator end = FieldValueList.end();

  for (; it != end; ++it)
    {
      mNodes.insert(CNetwork::INSTANCE->lookupNode(it->toId(), mLocalScope));
    }
}

void CNodeElementSelector::nodeWithDBFieldWithin()
{
  mNodes.clear();
  CFieldValueList FieldValueList;

  if (mpDBFieldValueList != NULL)
    CQuery::in(mDBTable, "pid", FieldValueList, mLocalScope, mDBField, *mpDBFieldValueList);
  else
    {
      CField Field = CSchema::INSTANCE.getTable(mDBTable).getField(mDBField);
      const std::map< CValueList::Type, CValueList > & ValueListMap = mpSelector->getDBFieldValues();
      std::map< CValueList::Type, CValueList >::const_iterator found = ValueListMap.find(Field.getType());

      if (found != ValueListMap.end())
        CQuery::in(mDBTable, "pid", FieldValueList, mLocalScope, mDBField, found->second);
    }

  CFieldValueList::const_iterator it = FieldValueList.begin();
  CFieldValueList::const_iterator end = FieldValueList.end();

  for (; it != end; ++it)
    {
      mNodes.insert(CNetwork::INSTANCE->lookupNode(it->toId(), mLocalScope));
    }
}

void CNodeElementSelector::nodeWithDBFieldNotWithin()
{
  mNodes.clear();
  CFieldValueList FieldValueList;

  if (mpDBFieldValueList != NULL)
    CQuery::notIn(mDBTable, "pid", FieldValueList, mLocalScope, mDBField, *mpDBFieldValueList);
  else
    {
      CField Field = CSchema::INSTANCE.getTable(mDBTable).getField(mDBField);
      const std::map< CValueList::Type, CValueList > & ValueListMap = mpSelector->getDBFieldValues();
      std::map< CValueList::Type, CValueList >::const_iterator found = ValueListMap.find(Field.getType());

      if (found != ValueListMap.end())
        CQuery::notIn(mDBTable, "pid", FieldValueList, mLocalScope, mDBField, found->second);
    }

  CFieldValueList::const_iterator it = FieldValueList.begin();
  CFieldValueList::const_iterator end = FieldValueList.end();

  for (; it != end; ++it)
    {
      mNodes.insert(CNetwork::INSTANCE->lookupNode(it->toId(), mLocalScope));
    }
}
