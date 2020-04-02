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

#include "sets/CSetContent.h"

#include "sets/CSetReference.h"
#include "sets/CEdgeElementSelector.h"
#include "sets/CNodeElementSelector.h"
#include "sets/CDBFieldSelector.h"
#include "sets/CSetOperation.h"
#include "sets/CSetList.h"
#include "utilities/CRandom.h"
#include "utilities/CLogger.h"

CSetContent::CSetContent()
  : CComputable()
  , mNodes()
  , mEdges()
  , mDBFieldValues()
  , mValid(true)
{}

CSetContent::CSetContent(const CSetContent & src)
  : CComputable(src)
  , mNodes(src.mNodes)
  , mEdges(src.mEdges)
  , mDBFieldValues(src.mDBFieldValues)
  , mValid(src.mValid)
{}

CSetContent::~CSetContent()
{}

// virtual
bool CSetContent::computeProtected()
{
  return false;
}

// static
CSetContent * CSetContent::create(const json_t * json)
{
  json_t * pContentType = json_object_get(json, "elementType");

  if (json_is_string(pContentType))
    {
      if (strcmp(json_string_value(pContentType), "edge") == 0)
        {
          return new CEdgeElementSelector(json);
        }
      else if (strcmp(json_string_value(pContentType), "node") == 0)
        {
          return new CNodeElementSelector(json);
        }
      else if (strcmp(json_string_value(pContentType), "dbField") == 0)
        {
          return new CDBFieldSelector(json);
        }
      else
        {
          CLogger::error("Set content: Invalid value for 'elementType'.");
          return NULL;
        }
    }

  pContentType = json_object_get(json, "operation");

  if (json_is_string(pContentType))
    {
      return new CSetOperation(json);
    }

  pContentType = json_object_get(json, "set");

  if (json_is_object(pContentType))
    {
      return new CSetReference(json);
    }

  CLogger::error("Set content: Invalid.");
  return NULL;
}

// static
/*
CSetContent * CSetContent::copy(const CSetContent * pSetContent)
{
  if (dynamic_cast< const CEdgeElementSelector * >(pSetContent) != NULL)
    return new CEdgeElementSelector(*static_cast< const CEdgeElementSelector * >(pSetContent));

  if (dynamic_cast< const CNodeElementSelector * >(pSetContent) != NULL)
    return new CNodeElementSelector(*static_cast< const CNodeElementSelector * >(pSetContent));

  if (dynamic_cast< const CDBFieldSelector * >(pSetContent) != NULL)
    return new CDBFieldSelector(*static_cast< const CDBFieldSelector * >(pSetContent));

  if (dynamic_cast< const CSetOperation * >(pSetContent) != NULL)
    return new CSetOperation(*static_cast<const  CSetOperation * >(pSetContent));

  if (dynamic_cast< const CSetReference * >(pSetContent) != NULL)
    return new CSetReference(*static_cast<const  CSetReference * >(pSetContent));

  return NULL;
}
*/

// static
void CSetContent::destroy(CSetContent *& pSetContent)
{
  if (pSetContent != NULL
      && dynamic_cast< CSet * >(pSetContent) == NULL)
    {
      delete pSetContent;
      pSetContent = NULL;
    }
}

const bool & CSetContent::isValid() const
{
  return mValid;
}

bool CSetContent::contains(CNode * pNode) const
{
  return binary_search(beginNodes(), endNodes(), pNode);
}

bool CSetContent::contains(CEdge * pEdge) const
{
  return binary_search(beginEdges(), endEdges(), pEdge);
}

bool CSetContent::contains(const CValueInterface & value) const
{
  std::map< CValueList::Type, CValueList >::const_iterator found = mDBFieldValues.find(value.getType());

  if (found != mDBFieldValues.end())
    return found->second.contains(value);

  return false;
}

std::vector< CEdge * >::const_iterator CSetContent::beginEdges() const
{
  return mEdges.begin();
}

std::vector< CEdge * >::const_iterator CSetContent::endEdges() const
{
  return mEdges.end();
}

std::vector< CNode * >::const_iterator CSetContent::beginNodes() const
{
  return mNodes.begin();
}

std::vector< CNode * >::const_iterator CSetContent::endNodes() const
{
  return mNodes.end();
}

// virtual
const std::vector< CEdge * > & CSetContent::getEdges() const
{
  return mEdges;
}

// virtual
const std::vector< CNode * > & CSetContent::getNodes() const
{
  return mNodes;
}

const std::map< CValueList::Type, CValueList > & CSetContent::getDBFieldValues() const
{
  return mDBFieldValues;
}

// virtual
std::vector< CEdge * > & CSetContent::getEdges()
{
  return mEdges;
}

// virtual
std::vector< CNode * > & CSetContent::getNodes()
{
  return mNodes;
}

std::map< CValueList::Type, CValueList > & CSetContent::getDBFieldValues()
{
  return mDBFieldValues;
}

void CSetContent::sampleMax(const size_t & max, CSetContent & sampled, CSetContent & notSampled) const
{
  const std::vector< CNode * > & Nodes = getNodes();
  const std::vector< CEdge * > & Edges = getEdges();
  sampled.mEdges.clear();
  sampled.mNodes.clear();
  sampled.mDBFieldValues.clear();

  notSampled.mEdges.clear();
  notSampled.mNodes.clear();
  notSampled.mDBFieldValues.clear();

  CRandom::uniform_real Percent(0.0, 1.0);

  // Sampling is only supported if we have either only nodes or only edges;
  if (size() == Nodes.size())
    {
      double Requested = max;
      double Available = Nodes.size();

      std::vector< CNode * >::const_iterator it = Nodes.begin();
      std::vector< CNode * >::const_iterator end = Nodes.end();

      for (; it != end; ++it)
        {
          if (Available <= Requested
              || (Requested > 0.5
                  && Percent(CRandom::G) < Requested / Available))
            {
              sampled.mNodes.push_back(*it);
              Requested -= 1.0;
            }
          else
            notSampled.mNodes.push_back(*it);

          Available -= 1.0;
        }
    }
  else if (size() == Edges.size())
    {
      double Requested = max;
      double Available = Edges.size();

      std::vector< CEdge * >::const_iterator it = Edges.begin();
      std::vector< CEdge * >::const_iterator end = Edges.end();

      for (; it != end; ++it)
        {
          if (Available <= Requested
              || (Requested > 0.5
                  && Percent(CRandom::G) < Requested / Available))
            {
              sampled.mEdges.push_back(*it);
              Requested -= 1.0;
            }
          else
            notSampled.mEdges.push_back(*it);

          Available -= 1.0;
        }
    }
}

void CSetContent::samplePercent(const double & percent, CSetContent & sampled, CSetContent & notSampled) const
{
  sampled.mEdges.clear();
  sampled.mNodes.clear();
  sampled.mDBFieldValues.clear();

  notSampled.mEdges.clear();
  notSampled.mNodes.clear();
  notSampled.mDBFieldValues.clear();

  CRandom::uniform_real Percent(0.0, 100.0);

  // Sampling is ony supported if we have either only nodes or only edges;
  if (size() == getNodes().size())
    {
      std::vector< CNode * >::const_iterator it = getNodes().begin();
      std::vector< CNode * >::const_iterator end = getNodes().end();

      for (; it != end; ++it)
        if (Percent(CRandom::G) < percent)
          sampled.mNodes.push_back(*it);
        else
          notSampled.mNodes.push_back(*it);
    }
  else if (size() == getEdges().size())
    {
      std::vector< CEdge * >::const_iterator it = getEdges().begin();
      std::vector< CEdge * >::const_iterator end = getEdges().end();

      for (; it != end; ++it)
        if (Percent(CRandom::G) < percent)
          sampled.mEdges.push_back(*it);
        else
          notSampled.mEdges.push_back(*it);
    }
}

size_t CSetContent::size() const
{
  return getEdges().size() + getNodes().size() + getDBFieldValues().size();
}
