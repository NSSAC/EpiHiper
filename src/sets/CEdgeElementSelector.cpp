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

#include <cstring>
#include <jansson.h>

#include "sets/CEdgeElementSelector.h"
#include "network/CNetwork.h"
#include "network/CEdge.h"
#include "network/CNode.h"
#include "actions/CActionQueue.h"
#include "utilities/CLogger.h"

CEdgeElementSelector::CEdgeElementSelector()
  : CSetContent()
  , mLeft()
  , mpValue(NULL)
  , mpValueList(NULL)
  , mpSelector(NULL)
  , mpComparison(NULL)
  , mpGetNode(NULL)
{}

CEdgeElementSelector::CEdgeElementSelector(const CEdgeElementSelector & src)
  : CSetContent(src)
  , mLeft(src.mLeft)
  , mpValue(src.mpValue != NULL ? new CValue(*src.mpValue) : NULL)
  , mpValueList(src.mpValueList != NULL ? new CValueList(*src.mpValueList) : NULL)
  , mpSelector(src.mpSelector != NULL ? src.mpSelector->copy() : NULL)
  , mpComparison(src.mpComparison)
  , mpGetNode(src.mpGetNode)
{}

CEdgeElementSelector::CEdgeElementSelector(const json_t * json)
  : CSetContent()
  , mLeft()
  , mpValue(NULL)
  , mpValueList(NULL)
  , mpSelector(NULL)
  , mpComparison(NULL)
  , mpGetNode(NULL)
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
                  "enum": ["withPropertyIn"]
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
  json_t * pValue = json_object_get(json, "elementType");

  if (json_is_string(pValue)
      && strcmp(json_string_value(pValue), "edge") != 0)
    {
      CLogger::error("Edge selector: Invalid value for 'elementType'.");
      return;
    }

  mPrerequisites.insert(&CActionQueue::getCurrentTick());

  pValue = json_object_get(json, "operator");

  if (!json_is_string(pValue))
    {
      // We need to return all edges
      mStatic = true;
      mValid = true;
      return;
    }

  if (strcmp(json_string_value(pValue), "==") == 0)
    {
      mpComparison = &operator==;
    }
  else if (strcmp(json_string_value(pValue), "!=") == 0)
    {
      mpComparison = &operator!=;
    }
  else if (strcmp(json_string_value(pValue), "<=") == 0)
    {
      mpComparison = &operator<=;
    }
  else if (strcmp(json_string_value(pValue), "<") == 0)
    {
      mpComparison = &operator<;
    }
  else if (strcmp(json_string_value(pValue), ">=") == 0)
    {
      mpComparison = &operator>=;
    }
  else if (strcmp(json_string_value(pValue), ">") == 0)
    {
      mpComparison = &operator>;
    }

  // Select edges where the edge property value comparison with the provided value is true.
  if (mpComparison != NULL)
    {
      mLeft.fromJSON(json_object_get(json, "left"));

      if (!mLeft.isValid())
        {
          CLogger::error("Edge selector: Invalid or missing value for 'left'.");
          return;
        }

      mpValue = new CValue(json_object_get(json, "right"));

      if (mpValue == NULL
          || !mpValue->isValid())
        {
          CLogger::error("Edge selector: Invalid or missing value for 'right'.");
          return;
        }

      if (mLeft.isReadOnly())
        mStatic = true;

      mValid = true;
      return;
    }

  // Select edges where the edge property value in in the provided list.
  if (strcmp(json_string_value(pValue), "withPropertyIn") == 0)
    {
      mLeft.fromJSON(json_object_get(json, "left"));

      if (!mLeft.isValid())
        {
          CLogger::error("Edge selector: Invalid or missing value for 'left'.");
          return;
        }

      mpValueList = new CValueList(json_object_get(json, "right"));

      if (mpValueList == NULL
          || !mpValueList->isValid())
      {
        CLogger::error("Edge selector: Invalid or missing value for 'right'.");
        return;
        }

      if (mLeft.isReadOnly())
        mStatic = true;

      mValid = true;
      return;
    }

  if (strcmp(json_string_value(pValue), "withTargetNodeIn") == 0)
    {
      mpGetNode = &CEdgeProperty::targetNode;
    }
  else if (strcmp(json_string_value(pValue), "withSourceNodeIn") == 0)
    {
      mpGetNode = &CEdgeProperty::sourceNode;
    }

  if (mpGetNode != NULL)
    {
      mpSelector = CSetContent::create(json_object_get(json, "selector"));

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

      CLogger::error("Edge selector: Invalid or missing value for 'selector'.");
      return;
    }

  CLogger::error("Edge selector: Invalid or missing value for 'operator'.");
  return;
}

// virtual
void CEdgeElementSelector::computeProtected()
{
  // We need to loop through all edges and select the ones fitting the specification
  std::vector< CEdge * > & Edges = getEdges();
  Edges.clear();

  if (!mValid)
    return;

  CEdge * pEdge = CNetwork::INSTANCE->beginEdge();
  CEdge * pEdgeEnd = CNetwork::INSTANCE->endEdge();

  if (mpValue != NULL)
    {
      for (; pEdge != pEdgeEnd; ++pEdge)
        if ((*mpComparison)(mLeft.propertyOf(pEdge), *mpValue))
          {
            Edges.push_back(pEdge);
          }

      return;
    }

  if (mpValueList != NULL)
    {
      for (; pEdge != pEdgeEnd; ++pEdge)
        if (mpValueList->contains(mLeft.propertyOf(pEdge)))
          {
            Edges.push_back(pEdge);
          }

      return;
    }

  if (mpSelector != NULL)
    {
      for (; pEdge != pEdgeEnd; ++pEdge)
        if (mpSelector->contains((*mpGetNode)(pEdge)))
          {
            Edges.push_back(pEdge);
          }

      return;
    }

  for (; pEdge != pEdgeEnd; ++pEdge)
    Edges.push_back(pEdge);
}
