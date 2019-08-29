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

#include "sets/CEdgeElementSelector.h"
#include "network/CNetwork.h"
#include "network/CEdge.h"
#include "network/CNode.h"

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
  , mpSelector(CSetContent::copy(src.mpSelector))
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
  if (mpSelector != NULL)
    delete mpSelector;
}

// virtual
void CEdgeElementSelector::fromJSON(const json_t * json)
{
  json_t * pValue = json_object_get(json, "elementType");

  mValid = (json_is_string(pValue) && strcmp(json_string_value(pValue), "edge") == 0);

  if (!mValid) return;

  pValue = json_object_get(json, "operator");

  if (!json_is_string(pValue))
    {
      // We need to return all edges
      return;
    }

  if (strcmp(json_string_value(pValue), "==") == 0)
    {
      mpComparison = &operator==;
    }
  else if(strcmp(json_string_value(pValue), "!=") == 0)
    {
      mpComparison = &operator!=;
    }
  else if(strcmp(json_string_value(pValue), "<=") == 0)
    {
      mpComparison = &operator<=;
    }
  else if(strcmp(json_string_value(pValue), "<") == 0)
    {
      mpComparison = &operator<;
    }
  else if(strcmp(json_string_value(pValue), ">=") == 0)
    {
      mpComparison = &operator>=;
    }
  else if(strcmp(json_string_value(pValue), ">") == 0)
    {
      mpComparison = &operator>;
    }

  // Select edges where the edge property value comparison with the provided value is true.
  if (mpComparison != NULL)
    {
      mLeft.fromJSON(json_object_get(json, "left"));
      mValid &= mLeft.isValid();
      mpValue = new CValue(json_object_get(json, "right"));
      mValid &= (mpValue != NULL && mpValue->isValid());

      return;
    }

  // Select edges where the edge property value in in the provided list.
  if (strcmp(json_string_value(pValue), "withPropertyIn") == 0)
    {
      mLeft.fromJSON(json_object_get(json, "left"));
      mValid &= mLeft.isValid();
      mpValueList = new CValueList(json_object_get(json, "right"));
      mValid &= (mpValueList != NULL && mpValueList->isValid());

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
      mValid &= (mpSelector != NULL && mpSelector->isValid());

      if (mValid)
        mPrerequisites.insert(mpSelector);

      return;
    }

  mValid = false;
  return;
}

// virtual
void CEdgeElementSelector::compute()
{
  // We need to loop through all edges and select the ones fitting the specification
  mEdges.clear();

  if (!mValid) return;

  CEdge * pEdge = CNetwork::INSTANCE->beginEdge();
  CEdge * pEdgeEnd = CNetwork::INSTANCE->endEdge();;

  if (mpValue != NULL)
    {
      for (; pEdge != pEdgeEnd; ++pEdge)
        if ((*mpComparison)(mLeft.propertyOf(pEdge), *mpValue))
          {
            mEdges.insert(pEdge);
          }

      return;
    }

  if (mpValueList != NULL)
    {
      for (; pEdge != pEdgeEnd; ++pEdge)
        if(mpValueList->contains(mLeft.propertyOf(pEdge)))
          {
            mEdges.insert(pEdge);
          }

      return;
    }

  if (mpSelector != NULL)
    {
      for (; pEdge != pEdgeEnd; ++pEdge)
        if(mpSelector->contains((*mpGetNode)(pEdge)))
          {
            mEdges.insert(pEdge);
          }

      return;
    }

  for (; pEdge != pEdgeEnd; ++pEdge)
    mEdges.insert(pEdge);
}


