// BEGIN: Copyright 
// Copyright (C) 2019 - 2021 Rector and Visitors of the University of Virginia 
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

size_t CSetContent::CDBFieldValues::size() const
{
  size_t Size = 0;

  std::map< CValueList::Type, CValueList >::const_iterator itMap = begin();
  std::map< CValueList::Type, CValueList >::const_iterator endMap = end();
  
  for (; itMap != endMap; ++itMap)
    Size += itMap->second.size();

  return Size;
}

CSetContent::CSetContent()
  : CComputable()
  , mContext()
  , mValid(true)
{
  mContext.init();
}

CSetContent::CSetContent(const CSetContent & src)
  : CComputable(src)
  , mContext(src.mContext)
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
  const SetContent & Active = getContext().Active();
  CDBFieldValues::const_iterator found = Active.dBFieldValues.find(value.getType());

  if (found != Active.dBFieldValues.end())
    return found->second.contains(value);

  return false;
}

std::vector< CEdge * >::const_iterator CSetContent::beginEdges() const
{
  return getContext().Active().edges.begin();
}

std::vector< CEdge * >::const_iterator CSetContent::endEdges() const
{
  return getContext().Active().edges.end();
}

std::vector< CNode * >::const_iterator CSetContent::beginNodes() const
{
  return getContext().Active().nodes.begin();
}

std::vector< CNode * >::const_iterator CSetContent::endNodes() const
{
  return getContext().Active().nodes.end();
}

// virtual
const std::vector< CEdge * > & CSetContent::getEdges() const
{
  return getContext().Active().edges;
}

// virtual
const std::vector< CNode * > & CSetContent::getNodes() const
{
  return getContext().Active().nodes;
}

const CSetContent::CDBFieldValues & CSetContent::getDBFieldValues() const
{
  return getContext().Active().dBFieldValues;
}

// virtual
std::vector< CEdge * > & CSetContent::getEdges()
{
  return getContext().Active().edges;
}

// virtual
std::vector< CNode * > & CSetContent::getNodes()
{
  return getContext().Active().nodes;
}

CSetContent::CDBFieldValues & CSetContent::getDBFieldValues()
{
  return getContext().Active().dBFieldValues;
}

void CSetContent::sampleMax(const size_t & max, CSetContent & sampled, CSetContent & notSampled) const
{
  SetContent & Sampled = sampled.getContext().Active();
  SetContent & NotSampled = notSampled.getContext().Active();

  const std::vector< CNode * > & Nodes = getNodes();
  const std::vector< CEdge * > & Edges = getEdges();


  Sampled.edges.clear();
  Sampled.nodes.clear();
  Sampled.dBFieldValues.clear();

  NotSampled.edges.clear();
  NotSampled.nodes.clear();
  NotSampled.dBFieldValues.clear();

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
                  && Percent(CRandom::G.Active()) < Requested / Available))
            {
              Sampled.nodes.push_back(*it);
              Requested -= 1.0;
            }
          else
            NotSampled.nodes.push_back(*it);

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
                  && Percent(CRandom::G.Active()) < Requested / Available))
            {
              Sampled.edges.push_back(*it);
              Requested -= 1.0;
            }
          else
            NotSampled.edges.push_back(*it);

          Available -= 1.0;
        }
    }
}

void CSetContent::samplePercent(const double & percent, CSetContent & sampled, CSetContent & notSampled) const
{
  SetContent & Sampled = sampled.getContext().Active();
  SetContent & NotSampled = notSampled.getContext().Active();
  
  Sampled.edges.clear();
  Sampled.nodes.clear();
  Sampled.dBFieldValues.clear();

  NotSampled.edges.clear();
  NotSampled.nodes.clear();
  NotSampled.dBFieldValues.clear();

  CRandom::uniform_real Percent(0.0, 100.0);

  // Sampling is only supported if we have either only nodes or only edges;
  if (size() == getNodes().size())
    {
      std::vector< CNode * >::const_iterator it = getNodes().begin();
      std::vector< CNode * >::const_iterator end = getNodes().end();

      for (; it != end; ++it)
        if (Percent(CRandom::G.Active()) < percent)
          Sampled.nodes.push_back(*it);
        else
          NotSampled.nodes.push_back(*it);
    }
  else if (size() == getEdges().size())
    {
      std::vector< CEdge * >::const_iterator it = getEdges().begin();
      std::vector< CEdge * >::const_iterator end = getEdges().end();

      for (; it != end; ++it)
        if (Percent(CRandom::G.Active()) < percent)
          Sampled.edges.push_back(*it);
        else
          NotSampled.edges.push_back(*it);
    }
}

size_t CSetContent::size() const
{
  return getEdges().size() + getNodes().size() + getDBFieldValues().size();
}

size_t CSetContent::totalSize() const
{
  const CContext< SetContent > & Context = getContext();

  size_t TotalSize = 0;
  const SetContent * pIt = Context.beginThread();
  const SetContent * pEnd = Context.endThread();

  for (; pIt != pEnd; ++pIt)
    TotalSize += pIt->edges.size() + pIt->nodes.size() + pIt->dBFieldValues.size();

  return TotalSize;
}

CContext< size_t > CSetContent::sizes() const
{
  CContext< size_t > Sizes;
  Sizes.init();

  Sizes.Master() = totalSize();

  const CContext< SetContent > & Context = getContext();

  const SetContent * pIt = Context.beginThread();
  const SetContent * pEnd = Context.endThread();
  size_t * pSize = Sizes.beginThread();

  for (; pIt != pEnd; ++pIt, ++pSize)
    *pSize =  pIt->edges.size() + pIt->nodes.size() + pIt->dBFieldValues.size();

  return Sizes;
}
