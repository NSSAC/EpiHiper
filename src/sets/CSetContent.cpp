// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2023 Rector and Visitors of the University of Virginia 
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

#include "sets/CSetContent.h"

#include "sets/CSetReference.h"
#include "sets/CEdgeElementSelector.h"
#include "sets/CNodeElementSelector.h"
#include "sets/CDBFieldSelector.h"
#include "sets/CSetOperation.h"
#include "sets/CSetList.h"
#include "utilities/CRandom.h"
#include "utilities/CLogger.h"
#include "utilities/CSimConfig.h"

bool CSetContent::Compare::operator()(const CSetContentPtr & lhs, const CSetContentPtr & rhs)
{
  return lhs->lessThan(*rhs);
}

size_t CSetContent::CDBFieldValues::size() const
{
  size_t Size = 0;

  std::map< CValueList::Type, CValueList >::const_iterator itMap = begin();
  std::map< CValueList::Type, CValueList >::const_iterator endMap = end();
  
  for (; itMap != endMap; ++itMap)
    Size += itMap->second.size();

  return Size;
}

CSetContent::CSetContent(const Type & type)
  : CComputable()
  , mType(type)
  , mContext()
{
  mContext.init();
  mValid = true;
}

CSetContent::CSetContent(const CSetContent & src)
  : CComputable(src)
  , mType(src.mType)
  , mContext(src.mContext)
{}

CSetContent::~CSetContent()
{}

// virtual
bool CSetContent::computeProtected()
{
  return true;
}

// static
CSetContent::CSetContentPtr CSetContent::create(const json_t * json)
{
  CSetContent * pNew = NULL;

  json_t * pContentType = json_object_get(json, "elementType");

  if (json_is_string(pContentType))
    {
      if (strcmp(json_string_value(pContentType), "edge") == 0)
        {
          pNew = new CEdgeElementSelector(json);
        }
      else if (strcmp(json_string_value(pContentType), "node") == 0)
        {
          pNew = new CNodeElementSelector(json);
        }
      else if (strcmp(json_string_value(pContentType), "dbField") == 0)
        {
          pNew = new CDBFieldSelector(json);
        }
      else
        {
          CLogger::error() << "Set content: Invalid value for 'elementType'. " << CSimConfig::jsonToString(json);
        }
    }
  else
    {
      pContentType = json_object_get(json, "operation");

      if (json_is_string(pContentType))
        {
          pNew = new CSetOperation(json);
        }

      if (pNew == NULL)
        {
          pContentType = json_object_get(json, "set");

          if (json_is_object(pContentType))
            {
              pNew = new CSetReference(json);
            }
        }

      if (pNew == NULL)
        CLogger::error() << "Set content: Invalid." << CSimConfig::jsonToString(json);
    }

  // Assure unique CSetContent is returned
  if (pNew == NULL)
    return CSetContentPtr(NULL);

  CSetContentPtr New(pNew);

  return *UniqueSetContent.insert(New).first;
}

bool CSetContent::lessThan(const CSetContent & rhs) const
{
  if (mType == rhs.mType)
    return lessThanProtected(rhs);

  return mType < rhs.mType;
}

void CSetContent::fromJSON(const json_t * json)
{
  mJSON = CSimConfig::jsonToString(json);
  fromJSONProtected(json);
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

  CRandom::uniform_real Probability(0.0, 1.0);

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
                  && Probability(CRandom::G.Active()) < Requested / Available))
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
                  && Probability(CRandom::G.Active()) < Requested / Available))
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
