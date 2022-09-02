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

#include <cstring>
#include <jansson.h>

#include "sets/CEdgeElementSelector.h"
#include "math/CObservable.h"
#include "db/CConnection.h"
#include "db/CFieldValue.h"
#include "db/CFieldValueList.h"
#include "db/CQuery.h"
#include "db/CSchema.h"
#include "network/CNetwork.h"
#include "network/CEdge.h"
#include "network/CNode.h"
#include "actions/CActionQueue.h"
#include "utilities/CLogger.h"
#include "utilities/CSimConfig.h"

CEdgeElementSelector::CEdgeElementSelector()
  : CSetContent()
  , mEdgeProperty()
  , mpValue(NULL)
  , mpValueList(NULL)
  , mpSelector(NULL)
  , mDBTable()
  , mDBField()
  , mpObservable(NULL)
  , mpDBFieldValue(NULL)
  , mpDBFieldValueList(NULL)
  , mpComparison(NULL)
  , mpCompute(NULL)
{}

CEdgeElementSelector::CEdgeElementSelector(const CEdgeElementSelector & src)
  : CSetContent(src)
  , mEdgeProperty(src.mEdgeProperty)
  , mpValue(src.mpValue != NULL ? new CValue(*src.mpValue) : NULL)
  , mpValueList(src.mpValueList != NULL ? new CValueList(*src.mpValueList) : NULL)
  , mpSelector(src.mpSelector != NULL ? src.mpSelector->copy() : NULL)
  , mDBTable(src.mDBTable)
  , mDBField(src.mDBField)
  , mpObservable(src.mpObservable != NULL ? new CObservable(*src.mpObservable) : NULL)
  , mpDBFieldValue(src.mpDBFieldValue != NULL ? new CFieldValue(*src.mpDBFieldValue) : NULL)
  , mpDBFieldValueList(src.mpDBFieldValueList != NULL ? new CFieldValueList(*src.mpDBFieldValueList) : NULL)
  , mpComparison(src.mpComparison)
  , mpCompute(src.mpCompute)
{}

CEdgeElementSelector::CEdgeElementSelector(const json_t * json)
  : CSetContent()
  , mEdgeProperty()
  , mpValue(NULL)
  , mpValueList(NULL)
  , mpSelector(NULL)
  , mDBTable()
  , mDBField()
  , mpObservable(NULL)
  , mpDBFieldValue(NULL)
  , mpDBFieldValueList(NULL)
  , mpComparison(NULL)
  , mpCompute(NULL)
{
  fromJSON(json);
}

CEdgeElementSelector::~CEdgeElementSelector()
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
CSetContent * CEdgeElementSelector::copy() const
{
  return new CEdgeElementSelector(*this);
}

// virtual
void CEdgeElementSelector::fromJSON(const json_t * json)
{
  /*
  "edgeElementSelector": {
      "$id": "#edgeElementSelector",
      "description": "The specification of edge elements of a set.",
      "type": "object",
      "allOf": [
        {
          "required": ["elementType"],
          "properties": {
            "elementType": {
              "type": "string",
              "enum": ["edge"]
            }
          }
        },
        {
          "oneOf": [
            {
              "description": "Select edges were the edge property value comparison with the provided value is true",
              "type": "object",
              "required": [
                "operator",
                "left",
                "right"
              ],
              "properties": {
                "operator": {"$ref": "#/definitions/comparisonOperator"},
                "left": {
                  "type": "object",
                  "required": ["edge"],
                  "properties": {
                    "edge": {"$ref": "#/definitions/edgeProperty"}
                  }
                },
                "right": {"$ref": "#/definitions/value"}
              }
            },
            {
              "description": "Select edges were the edge property value in in the provided list",
              "type": "object",
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
                  "required": ["edge"],
                  "properties": {
                    "edge": {"$ref": "#/definitions/edgeProperty"}
                  }
                },
                "right": {"$ref": "#/definitions/valueList"}
              }
            },
            {
              "description": "Select edges were either target or source node is in the given set of nodes.",
              "type": "object",
              "required": [
                "operator",
                "selector"
              ],
              "properties": {
                "operator": {
                  "description": "",
                  "type": "string",
                  "enum": [
                    "withTargetNodeIn",
                    "withSourceNodeIn"
                  ]
                },
                "selector": {"$ref": "#/definitions/setContent"}
              }
            }
          ]
        }
      ]
    }
  */

  mValid = false; // DONE
  mStatic = true;
  mPrerequisites.clear();
  json_t * pValue = json_object_get(json, "elementType");

  if (json_is_string(pValue)
      && strcmp(json_string_value(pValue), "edge") != 0)
    {
      CLogger::error() << "Edge selector: Invalid or missing value 'elementType' for " << CSimConfig::jsonToString(json);
      return;
    }

  mPrerequisites.insert(&CActionQueue::getCurrentTick());

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
             "right": {"$ref": "#/definitions/valueList"}
           }
         },
           */

          mEdgeProperty.fromJSON(json_object_get(json, "left"));

          if (mEdgeProperty.isValid())
            {
              mpValueList = new CValueList(json_object_get(json, "right"));

              if (mpValueList != NULL
                  && !mpValueList->isValid())
                {
                  CLogger::error() << "Edge selector: Invalid or missing value 'right' for " << CSimConfig::jsonToString(json);
                  return;
                }

              if (strcmp(Operator, "not in") == 0)
                mpCompute = &CEdgeElementSelector::propertyNotIn;
              else
                mpCompute = &CEdgeElementSelector::propertyIn;

              mStatic &= mEdgeProperty.isReadOnly();
              mValid = true;
              return;
            }

          if (strcmp(Operator, "in") == 0)
            {
              CConnection::setRequired(true);
              mpCompute = &CEdgeElementSelector::withDBFieldWithin;
            }
          else if (strcmp(Operator, "not in") == 0)
            {
              CConnection::setRequired(true);
              mpCompute = &CEdgeElementSelector::withDBFieldNotWithin;
            }
          else
            {
              CLogger::error() << "Edge selector: Invalid or missing value 'left' for " << CSimConfig::jsonToString(json);
              return;
            }
         }
      else if (strcmp(json_string_value(pValue), "withTargetNodeIn") == 0)
        {
          mpCompute = &CEdgeElementSelector::withTargetNodeIn;
        }
      else if (strcmp(json_string_value(pValue), "withSourceNodeIn") == 0)
        {
          mpCompute = &CEdgeElementSelector::withSourceNodeIn;
        }
    }
  else
    {
      // We do not have an operator, i.e., we have either all edges or all edges with a table.
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
          mpCompute = &CEdgeElementSelector::inDBTable;
          mValid = true;
          return;
        }
      else
        {
          mpCompute = &CEdgeElementSelector::all;
          mValid = true;
          return;
        }

      CLogger::error() << "Edge selector: Invalid or missing value 'operator' for " << CSimConfig::jsonToString(json);
      return;
    }

  if (mpCompute == &CEdgeElementSelector::withDBFieldWithin
      || mpCompute == &CEdgeElementSelector::withDBFieldNotWithin)
    {
      /*
        {
          "description": "A filter selecting edge from the external location trait database.",
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
          CLogger::error() << "Edge selector: Invalid or missing value 'table' for " << CSimConfig::jsonToString(json);
          return;
        }

      const CField & Field = Table.getField(mDBField);

      if (!Field.isValid())
        {
          CLogger::error() << "Edge selector: Invalid or missing value 'field' for " << CSimConfig::jsonToString(json);
          return;
        }

      CFieldValueList FieldValueList(json_object_get(json, "right"), Field.getType());

      if (FieldValueList.isValid())
        {
          mpDBFieldValueList = new CFieldValueList(FieldValueList);
          mStatic &= true;
          mValid = true;
          return;
        }

      mpSelector = CSetContent::create(json_object_get(json, "right"));

      if (mpSelector != NULL
          && mpSelector->isValid())
        {
          mPrerequisites.insert(mpSelector);
          mStatic &= mpSelector->isStatic();
          mValid = true;
          return;
        }

      if (mpSelector != NULL)
        {
          delete mpSelector;
          mpSelector = NULL;
        }

      CLogger::error() << "Edge selector: Invalid or missing value 'right' for " << CSimConfig::jsonToString(json);
      return;
    }

  if (mpCompute == &CEdgeElementSelector::withTargetNodeIn
      || mpCompute == &CEdgeElementSelector::withSourceNodeIn)
    {
      mpSelector = CSetContent::create(json_object_get(json, "selector"));

      if (mpSelector != NULL
          && mpSelector->isValid())
        {
          mPrerequisites.insert(mpSelector);
          mStatic &= mpSelector->isStatic();
          mValid = true;
          return;
        }

      if (mpSelector != NULL)
        {
          delete mpSelector;
          mpSelector = NULL;
        }

      CLogger::error() << "Edge selector: Invalid or missing value 'selector' for " << CSimConfig::jsonToString(json);
      return;
    }

  // Select edges where the edge property value comparison with the provided value is true.
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

      mEdgeProperty.fromJSON(json_object_get(json, "left"));
      
      if (mEdgeProperty.isValid())
        {
          mpValue = new CValue(json_object_get(json, "right"));

          if (mpValue != NULL
              && mpValue->isValid())
            {
              mpCompute = &CEdgeElementSelector::propertySelection;
              mStatic &= mEdgeProperty.isReadOnly();
              mValid = true;
              return;
            }

          CLogger::error() << "Edge selector: Invalid or missing value 'right' for " << CSimConfig::jsonToString(json);
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
          CLogger::error() << "Edge selector: Invalid or missing value 'table' for " << CSimConfig::jsonToString(json);
          return;
        }

      const CField & Field = Table.getField(mDBField);

      if (!Field.isValid())
        {
          CLogger::error() << "Edge selector: Invalid or missing value 'field' for " << CSimConfig::jsonToString(json);
          return;
        }


      CConnection::setRequired(true);
      mpCompute = &CEdgeElementSelector::withDBFieldSelection;

      mpObservable = CObservable::get(json_object_get(json, "right"));

      if (mpObservable != NULL
          && mpObservable->isValid())
        {
          mPrerequisites.insert(mpObservable);
          mStatic &= mpObservable->isStatic();
          mValid = true;
          return;
        }

      CFieldValue FieldValue(json_object_get(json, "right"), Field.getType());

      if (FieldValue.isValid())
        {
          mpDBFieldValue = new CFieldValue(FieldValue);
          mStatic = true;
          mValid = true;
          return;
        }
    }

  CLogger::error() << "Edge selector: Invalid or missing value 'operator' for " << CSimConfig::jsonToString(json);
  return;
}

// virtual
bool CEdgeElementSelector::computeProtected()
{
  if (mValid
      && mpCompute != NULL)
    return (this->*mpCompute)();

  return false;
}

bool CEdgeElementSelector::all()
{
  std::vector< CEdge * > & Edges = getEdges();

  if (Edges.empty())
    {
      Edges.resize(CNetwork::Context.Active().getLocalEdgeCount());
      CEdge * pEdge = CNetwork::Context.Active().beginEdge();
      std::vector< CEdge * >::iterator it = Edges.begin();
      std::vector< CEdge * >::iterator end = Edges.end();

      for (; it != end; ++it, ++pEdge)
        *it = pEdge;
    }

  CLogger::debug() << "CEdgeElementSelector: all returned '" << Edges.size() << "' edges.";
  return true;

}
bool CEdgeElementSelector::propertySelection()
{
  std::vector< CEdge * > & Edges = getEdges();
  Edges.clear();

  CEdge * pEdge = CNetwork::Context.Active().beginEdge();
  CEdge * pEdgeEnd = CNetwork::Context.Active().endEdge();

  for (; pEdge != pEdgeEnd; ++pEdge)
    if (mpComparison(mEdgeProperty.propertyOf(pEdge), *mpValue))
      Edges.push_back(pEdge);


  CLogger::debug() << "CEdgeElementSelector: propertySelection returned '" << Edges.size() << "' edges.";
  return true;
}

bool CEdgeElementSelector::propertyIn()
{
  std::vector< CEdge * > & Edges = getEdges();
  Edges.clear();

  CEdge * pEdge = CNetwork::Context.Active().beginEdge();
  CEdge * pEdgeEnd = CNetwork::Context.Active().endEdge();

  for (; pEdge != pEdgeEnd; ++pEdge)
    if (mpValueList->contains(mEdgeProperty.propertyOf(pEdge)))
      Edges.push_back(pEdge);

  CLogger::debug() << "CEdgeElementSelector: propertyIn returned '" << Edges.size() << "' edges.";
  return true;
}

bool CEdgeElementSelector::propertyNotIn()
{
  std::vector< CEdge * > & Edges = getEdges();
  Edges.clear();

  CEdge * pEdge = CNetwork::Context.Active().beginEdge();
  CEdge * pEdgeEnd = CNetwork::Context.Active().endEdge();

  for (; pEdge != pEdgeEnd; ++pEdge)
    if (!mpValueList->contains(mEdgeProperty.propertyOf(pEdge)))
      Edges.push_back(pEdge);

  CLogger::debug() << "CEdgeElementSelector: propertyNotIn returned '" << Edges.size() << "' edges.";
  return true;
}

bool CEdgeElementSelector::withTargetNodeIn()
{
  std::vector< CEdge * > & Edges = getEdges();
  Edges.clear();

  const std::vector< CNode * > & Nodes = mpSelector->getNodes();
  CLogger::debug() << "CEdgeElementSelector: withTargetNodeIn nodes " << Nodes.size();
  const CNetwork & Active = CNetwork::Context.Active();

  if (!Nodes.empty())
    {
      std::vector< CNode * >::const_iterator itNode = Nodes.begin();
      std::vector< CNode * >::const_iterator endNode = Nodes.end();

      for (; itNode != endNode; ++itNode)
        {
          // We do not have edge information for remote nodes in an MPI environment and thus and must always ignore it.
          if (Active.isRemoteNode(*itNode))
            continue;

          CEdge * pEdge = (*itNode)->Edges;
          CEdge * pEdgeEnd = pEdge + (*itNode)->EdgesSize;

          for (;pEdge != pEdgeEnd; ++pEdge)
            Edges.push_back(pEdge);
        }

      // Sorting is not necessarry since the nodes are sorted
      // std::sort(Edges.begin(), Edges.end());
    }

  CLogger::debug() << "CEdgeElementSelector: withTargetNodeIn returned " << Edges.size() << " edges.";
  return true;
}

bool CEdgeElementSelector::withSourceNodeIn()
{
  std::vector< CEdge * > & Edges = getEdges();
  Edges.clear();

  const std::vector< CNode * > & Nodes = mpSelector->getNodes();
  CLogger::debug() << "CEdgeElementSelector: withSourceNodeIn nodes " << Nodes.size();

  if (!Nodes.empty())
    {
      std::vector< CNode * >::const_iterator itNode = Nodes.begin();
      std::vector< CNode * >::const_iterator endNode = Nodes.end();

      for (; itNode != endNode; ++itNode)
        {
          CNode::sOutgoingEdges & OutgoingEdges = (*itNode)->OutgoingEdges.Active();
          CEdge ** pEdge = OutgoingEdges.pEdges;
          CEdge ** pEdgeEnd = pEdge + OutgoingEdges.Size;

          CLogger::debug() << "CEdgeElementSelector::withSourceNodeIn: node " << (*itNode)->id << " edges " << OutgoingEdges.Size;

          for (;pEdge != pEdgeEnd; ++pEdge)
            Edges.push_back(*pEdge);
        }

      std::sort(Edges.begin(), Edges.end());
    }

  CLogger::debug() << "CEdgeElementSelector: withSourceNodeIn returned " << Edges.size() << " edges.";
  return true;
}

bool CEdgeElementSelector::inDBTable()
{
  bool success = false;
  std::vector< CEdge * > & Edges = getEdges();
  Edges.clear();

#ifdef USE_LOCATION_ID
  CFieldValueList FieldValueList;
  success = CQuery::all(mDBTable, "lid", FieldValueList, false);

  CEdge * pEdge = CNetwork::Context.Active().beginEdge();
  CEdge * pEdgeEnd = CNetwork::Context.Active().endEdge();

  for (; pEdge != pEdgeEnd; ++pEdge)
    if (FieldValueList.contains(pEdge->locationId))
      Edges.push_back(pEdge);

  CLogger::debug() << "CEdgeElementSelector: inDBTable returned " << Edges.size() << " edges.";
#endif

  return success;
}

bool CEdgeElementSelector::withDBFieldSelection()
{
  bool success = false;
  std::vector< CEdge * > & Edges = getEdges();
  Edges.clear();

#ifdef USE_LOCATION_ID
  CFieldValueList FieldValueList;

  if (mpObservable)
    success = CQuery::where(mDBTable, "lid", FieldValueList, false, mDBField, *mpObservable, mSQLComparison);
  else
    success = CQuery::where(mDBTable, "lid", FieldValueList, false, mDBField, *mpDBFieldValue, mSQLComparison);

  CEdge * pEdge = CNetwork::Context.Active().beginEdge();
  CEdge * pEdgeEnd = CNetwork::Context.Active().endEdge();

  for (; pEdge != pEdgeEnd; ++pEdge)
    if (FieldValueList.contains(pEdge->locationId))
      Edges.push_back(pEdge);

  CLogger::debug() << "CEdgeElementSelector: withDBFieldSelection returned '" << Edges.size() << "' edges.";
#endif

  return success;
}

bool CEdgeElementSelector::withDBFieldWithin()
{
  bool success = false;

#ifdef USE_LOCATION_ID
  std::vector< CEdge * > & Edges = getEdges();
  Edges.clear();

  CFieldValueList FieldValueList;

  if (mpDBFieldValueList != NULL)
    success = CQuery::in(mDBTable, "lid", FieldValueList, false, mDBField, *mpDBFieldValueList);
  else
    {
      CField Field = CSchema::INSTANCE.getTable(mDBTable).getField(mDBField);
      const CDBFieldValues & ValueListMap = mpSelector->getDBFieldValues();
      CDBFieldValues::const_iterator found = ValueListMap.find(Field.getType());

      if (found != ValueListMap.end())
        success = CQuery::in(mDBTable, "lid", FieldValueList, false, mDBField, found->second);
    }

  CEdge * pEdge = CNetwork::Context.Active().beginEdge();
  CEdge * pEdgeEnd = CNetwork::Context.Active().endEdge();

  for (; pEdge != pEdgeEnd; ++pEdge)
    if (FieldValueList.contains(pEdge->locationId))
      Edges.push_back(pEdge);

  CLogger::debug() << "CEdgeElementSelector: withDBFieldWithin returned '" << Edges.size() << "' edges.";
#endif

  return success;
}

bool CEdgeElementSelector::withDBFieldNotWithin()
{
  bool success = false;
  std::vector< CEdge * > & Edges = getEdges();
  Edges.clear();

#ifdef USE_LOCATION_ID
  CFieldValueList FieldValueList;

  if (mpDBFieldValueList != NULL)
    success = CQuery::notIn(mDBTable, "lid", FieldValueList, false, mDBField, *mpDBFieldValueList);
  else
    {
      CField Field = CSchema::INSTANCE.getTable(mDBTable).getField(mDBField);
      const CDBFieldValues & ValueListMap = mpSelector->getDBFieldValues();
      CDBFieldValues::const_iterator found = ValueListMap.find(Field.getType());

      if (found != ValueListMap.end())
        success = CQuery::notIn(mDBTable, "lid", FieldValueList, false, mDBField, found->second);
    }

  CEdge * pEdge = CNetwork::Context.Active().beginEdge();
  CEdge * pEdgeEnd = CNetwork::Context.Active().endEdge();

  for (; pEdge != pEdgeEnd; ++pEdge)
    if (FieldValueList.contains(pEdge->locationId))
      Edges.push_back(pEdge);

  CLogger::debug() << "CEdgeElementSelector: withDBFieldNotWithin returned '" << Edges.size() << "' edges.";
#endif

  return success;
}
