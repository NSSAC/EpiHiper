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
#include "network/CNetwork.h"

// static
bool CSetContent::Compare::operator()(const CSetContent::shared_pointer & lhs, const CSetContent::shared_pointer & rhs) const
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
  , mContext()
  , mType(type)
  , mJSON()
  , mScope(CSetContent::Scope::local)
{
  mContext.init();
  mValid = true;
}

CSetContent::CSetContent(const CSetContent & src)
  : CComputable(src)
  , mContext(src.mContext)
  , mType(src.mType)
  , mJSON(src.mJSON)
  , mScope(src.mScope)
{}

CSetContent::~CSetContent()
{}

// virtual
bool CSetContent::computeProtected()
{
  mContext.Active().clear();

  bool success = computeSetContent();

  mContext.Active().sync();

  return success;
}

void CSetContent::SetContent::clear()
{
  edges.clear();
  mNodes.clear();
  dBFieldValues.clear();

  globalNodes.mBegin = mNodes.begin();
  globalNodes.mEnd = mNodes.end();
  globalNodes.mSize = mNodes.size();

  localNodes = globalNodes;
}

void CSetContent::SetContent::sync()
{
  globalNodes.mBegin = mNodes.begin();
  globalNodes.mEnd = mNodes.end();
  globalNodes.mSize = mNodes.size();

  localNodes = globalNodes;

  if (localNodes.mSize)
    {
      CNode * pFirstLocalNode = CNetwork::Context.Active().beginNode();

      while (*localNodes.mBegin < pFirstLocalNode)
        {
          ++localNodes.mBegin;
          --localNodes.mSize;
        }

      CNode * pBeyondLocalNode = CNetwork::Context.Active().endNode();

      // We must be able to dereference the iterator
      --localNodes.mEnd;

      while (pBeyondLocalNode <= *localNodes.mEnd)
        {
          --localNodes.mEnd;
          --localNodes.mSize;
        }

      ++localNodes.mEnd;
    }
}

CSetContent::SetContent::Nodes & CSetContent::SetContent::nodes(const CSetContent::Scope & scope)
{
  if (scope == Scope::local)
    return localNodes;

  return globalNodes;
}
  

// virtual 
CSetContent::FilterType CSetContent::filterType() const
{
  return FilterType::none;
}

// virtual 
bool CSetContent::filter(const CNode * /* pNode */) const
{
  return false;
} 

// virtual 
bool CSetContent::filter(const CEdge * /* pEdge */) const
{
  return false;
} 
 
// static
CSetContent::shared_pointer CSetContent::create(const json_t * json)
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
          CLogger::error("Set content: Invalid value for 'elementType'. {}", CSimConfig::jsonToString(json));
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
        CLogger::error("Set content: Invalid. {}", CSimConfig::jsonToString(json));
    }

  // Assure unique CSetContent is returned
  if (pNew == NULL)
    return shared_pointer(NULL);

  shared_pointer New(pNew);

  return *Unique.insert(New).first;
}

bool CSetContent::lessThan(const CSetContent & rhs) const
{
  if (mType != rhs.mType)
    return mType < rhs.mType;

  if (mScope != rhs.mScope)
    return mScope < rhs.mScope;

  return lessThanProtected(rhs);
}

void CSetContent::fromJSON(const json_t * json)
{
  mJSON = CSimConfig::jsonToString(json);
  fromJSONProtected(json);
}

const std::string & CSetContent::getJSON() const
{
  return mJSON;
}

bool CSetContent::contains(CNode * pNode) const
{
  const SetContent & Content = activeContent();
  const NodeContent & Nodes = (mScope == Scope::local) ? Content.localNodes : Content.globalNodes;

  return binary_search(Nodes.begin(), Nodes.end(), pNode);
}

bool CSetContent::contains(CEdge * pEdge) const
{
  const SetContent & Content = activeContent();

  return binary_search(Content.edges.begin(), Content.edges.end(), pEdge);
}

bool CSetContent::contains(const CValueInterface & value) const
{
  const SetContent & Active = activeContent();
  CDBFieldValues::const_iterator found = Active.dBFieldValues.find(value.getType());

  if (found != Active.dBFieldValues.end())
    return found->second.contains(value);

  return false;
}

const CSetContent::SetContent & CSetContent::activeContent() const
{
  return getContext().Active();
}

CSetContent::SetContent & CSetContent::activeContent()
{
  return getContext().Active();
}

const CSetContent::NodeContent & CSetContent::getNodeContent(const CSetContent::Scope & scope) const
{
  if (scope == Scope::local)
    return activeContent().localNodes;

  if (mScope != Scope::global)
    CLogger::error("CSetContent::beginNode: Conflicting scope global requested for '{}'.", mJSON);

  return activeContent().globalNodes;
}

void CSetContent::sampleMax(const size_t & max, CSetContent & sampled, CSetContent & notSampled) const
{
  SetContent & Sampled = sampled.activeContent();
  SetContent & NotSampled = notSampled.activeContent();

  Sampled.clear();
  NotSampled.clear();

  if (size() == 0)
    return;

  CRandom::uniform_real Probability(0.0, 1.0);
  CRandom::generator_t & Generator = CRandom::G.Active();
  const SetContent & Active = activeContent();

  // Sampling is only supported if we have either only nodes or only edges;
  if (size() == Active.mNodes.size())
    {
      const NodeContent & Nodes = getNodeContent(Scope::local);

      double Requested = max;
      double Available = Nodes.size();

      std::vector< CNode * >::const_iterator it = Nodes.begin();
      std::vector< CNode * >::const_iterator end = Nodes.end();

      for (; it != end; ++it)
        {
          if (Available <= Requested
              || (Requested > 0.5
                  && Probability(Generator) < Requested / Available))
            {
              Sampled.mNodes.push_back(*it);
              Requested -= 1.0;
            }
          else
            NotSampled.mNodes.push_back(*it);

          Available -= 1.0;
        }
    }
  else if (size() == Active.edges.size())
    {
      const std::vector< CEdge * > & Edges = Active.edges;

      double Requested = max;
      double Available = Edges.size();

      std::vector< CEdge * >::const_iterator it = Edges.begin();
      std::vector< CEdge * >::const_iterator end = Edges.end();

      for (; it != end; ++it)
        {
          if (Available <= Requested
              || (Requested > 0.5
                  && Probability(Generator) < Requested / Available))
            {
              Sampled.edges.push_back(*it);
              Requested -= 1.0;
            }
          else
            NotSampled.edges.push_back(*it);

          Available -= 1.0;
        }
    }

  Sampled.sync();
  NotSampled.sync();
}

void CSetContent::samplePercent(const double & percent, CSetContent & sampled, CSetContent & notSampled) const
{
  SetContent & Sampled = sampled.activeContent();
  SetContent & NotSampled = notSampled.activeContent();
  
  Sampled.clear();
  NotSampled.clear();

  if (size() == 0)
    return;

  CRandom::uniform_real Percent(0.0, 100.0);
  CRandom::generator_t & Generator = CRandom::G.Active();
  const SetContent & Active = activeContent();

  // Sampling is only supported if we have either only nodes or only edges;
  if (size() == Active.mNodes.size())
    {
      const NodeContent & Nodes = getNodeContent(Scope::local);

      std::vector< CNode * >::const_iterator it = Nodes.begin();
      std::vector< CNode * >::const_iterator end = Nodes.end();

      for (; it != end; ++it)
        if (Percent(Generator) < percent)
          Sampled.mNodes.push_back(*it);
        else
          NotSampled.mNodes.push_back(*it);
    }
  else if (size() == Active.edges.size())
    {
      std::vector< CEdge * >::const_iterator it = Active.edges.begin();
      std::vector< CEdge * >::const_iterator end = Active.edges.end();

      for (; it != end; ++it)
        if (Percent(Generator) < percent)
          Sampled.edges.push_back(*it);
        else
          NotSampled.edges.push_back(*it);
    }

  Sampled.sync();
  NotSampled.sync();
}

size_t CSetContent::size() const
{
  const SetContent & Active = activeContent();

  return Active.edges.size() + Active.mNodes.size() + Active.dBFieldValues.size();
}

size_t CSetContent::totalSize() const
{
  const CContext< SetContent > & Context = getContext();

  size_t TotalSize = 0;
  const SetContent * pIt = Context.beginThread();
  const SetContent * pEnd = Context.endThread();

  for (; pIt != pEnd; ++pIt)
    TotalSize += pIt->edges.size() + pIt->localNodes.size() + pIt->dBFieldValues.size();

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
    *pSize =  pIt->edges.size() + pIt->mNodes.size() + pIt->dBFieldValues.size();

  return Sizes;
}

void CSetContent::setScope(const CSetContent::Scope & scope)
{
  mScope = scope;

  if (mScope == Scope::global)
    setScopeProtected();
}

const CSetContent::Scope & CSetContent::getScope() const
{
  return mScope;
}

