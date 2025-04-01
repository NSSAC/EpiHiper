// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2023 - 2024 Rector and Visitors of the University of Virginia 
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

#ifndef SRC_SET_CSETCOLLECOTR_H_
#define SRC_SET_CSETCOLLECOTR_H_

#include <map>
#include <algorithm>

#include <bitset>

#include "sets/CNodeElementSelector.h"
#include "sets/CEdgeElementSelector.h"
#include "network/CNetwork.h"
#include "network/CNode.h"
#include "network/CEdge.h"
#include "utilities/CLogger.h"


#define USE_CSETCOLLECTOR 1

// #define USE_BITSET 1
#define USE_HASH_MAP 1

class CSetCollectorInterface
{
public:
  virtual ~CSetCollectorInterface(){};

  virtual bool isEnabled() const
  {
    return false;
  }
  
  virtual std::string getComputableId() const
  {
    return "invalid";
  }

  virtual bool record(CNode * /* pNode */)
  {
    return false;
  }

  virtual bool record(CEdge * /* pEdge */)
  {
    return false;
  }

  virtual bool apply() = 0;

  virtual void enable() = 0;

  virtual void disable() = 0;
};

template < class element_type, class selector > class CSetCollector : public CSetCollectorInterface
{
public:
  CSetCollector(selector * pSelector);

  virtual ~CSetCollector() {};

  virtual bool isEnabled() const override;
  
  virtual std::string getComputableId() const override;

  virtual bool record(element_type * pType) override;
 
  virtual bool apply() override;

  virtual void enable() override;

  virtual void disable() override;

private:
#ifdef USE_HASH_MAP
  typedef std::unordered_map< element_type *, int > changes;
#else
  typedef std::map< element_type *, int > changes;
#endif

  typedef std::vector< element_type * > sorted_vector;

  struct context {
#ifdef USE_BITSET
    std::bitset< 546356 > insert;
    std::bitset< 546356 > erase;
#else
    changes Changes;
#endif

    bool enabled = false;
  };

  sorted_vector & getSet() const;
  element_type * offset() const;

  selector * mpSelector;
  CContext< context > mContext;
};

template < class element_type, class selector > 
CSetCollector< element_type, selector >::CSetCollector(selector * pSelector)
  : mpSelector(pSelector)
  , mContext()
{
  mContext.init();
}

// virtual 
template < class element_type, class selector > 
bool CSetCollector< element_type, selector >::isEnabled() const
{
  return mContext.Active().enabled;
}

// virtual 
template < class element_type, class selector > 
std::string CSetCollector< element_type, selector >::getComputableId() const
{
  if (mpSelector)
    return mpSelector->getComputableId();

  return CSetCollectorInterface::getComputableId();
}


template < class element_type, class selector > 
bool CSetCollector< element_type, selector >::record(element_type * pType)
{
  context & Context = mContext.Active();

  if (!Context.enabled)
    return false;

#ifdef USE_BITSET
  size_t Index = pType - offset();

  if (mpSelector->filter(pType))
    {
      Context.insert.set(Index);
      Context.erase.reset(Index);
    }
    
  else
    {
      Context.insert.reset(Index);
      Context.erase.set(Index);
    }

#else
  typename changes::iterator found = Context.Changes.emplace(pType, 0).first;

  if (mpSelector->filter(pType))
    found->second = 1;
  else
    found->second = -1;
#endif

  return true;
}

template < class element_type, class selector > 
inline bool CSetCollector< element_type, selector >::apply()
{
  sorted_vector & Set = getSet();
  context & Context = mContext.Active();

  if (!Context.enabled)
    return false;

  sorted_vector Insert;
  sorted_vector Erase;

  ENABLE_TRACE(size_t count = 0;)
  
#ifdef USE_BITSET
  if (Context.insert.any())
    {
      size_t index = 0;
      element_type * pOffset = offset();

      for (; index < 546356; ++index, ++pOffset)
        if (Context.insert[index])
          {
            ENABLE_TRACE(++count;)
            Insert.push_back(pOffset);
          }

      Context.insert.reset();
    }

  if (Context.erase.any())
    {
      size_t index = 0;
      element_type * pOffset = offset();

      for (; index < 546356; ++index, ++pOffset)
        if (Context.erase[index])
          {
            ENABLE_TRACE(++count;)
            Erase.push_back(pOffset);
          }

      Context.erase.reset();
    }
#else
  ENABLE_TRACE(count = Context.Changes.size();)

  for (const typename changes::value_type & change: Context.Changes)
    if (change.second > 0)
      Insert.push_back(change.first);
    else if (change.second < 0)
      Erase.push_back(change.first);

#ifdef USE_HASH_MAP
  std::sort(Insert.begin(), Insert.end());
  std::sort(Erase.begin(), Erase.end());
#endif

  Context.Changes.clear();
#endif

  ENABLE_TRACE(CLogger::trace("CSetCollector::apply: changes '{}' => erase '{}' , insert '{}'.", count, Erase.size(), Insert.size()););

  sorted_vector temp;
  sorted_vector * pIn = &Set;
  sorted_vector * pOut = &temp;

  // Erase elements
  if (!Erase.empty())
    {
      std::set_difference(pIn->begin(), pIn->end(), Erase.begin(), Erase.end(), std::back_inserter(*pOut));
      ENABLE_TRACE(CLogger::trace("CSetCollector::apply: difference returned '{}' - '{}' = '{}'.", pIn->size(), Erase.size(), pOut->size()););
      std::swap(pIn, pOut);
    }

  // Insert elements
  if (!Insert.empty())
    {
      pOut->clear();
      std::set_union(pIn->begin(), pIn->end(), Insert.begin(), Insert.end(), std::back_inserter(*pOut));
      ENABLE_TRACE(CLogger::trace("CSetCollector::apply: union returned '{}' + '{}' = '{}'.", pIn->size(), Insert.size(), pOut->size()););
      std::swap(pIn, pOut);
    }

  if (pIn != &Set)
    Set = temp;

  mpSelector->activeContent().sync();
  
  CLogger::debug("CSetCollector::apply: returned '{}' elements.",Set.size());

  return true;
}

template < class element_type, class selector > 
void CSetCollector< element_type, selector >::enable()
{
  context & Context = mContext.Active();

  Context.enabled = true;

#ifdef USE_BITSET
#else
  Context.Changes.clear();
#endif

  // For the case that we are called single threaded
  if (mContext.isMaster(&Context))
    {
      context * pIt = mContext.beginThread();
      context * pEnd = mContext.endThread();

      for (; pIt != pEnd; ++pIt)
        if (mContext.isThread(pIt))
          *pIt = Context;
    }
}

template < class element_type, class selector > 
void CSetCollector< element_type, selector >::disable()
{
  context & Context = mContext.Active();

  Context.enabled = false;

#ifdef USE_BITSET
#else
  Context.Changes.clear();
#endif

  // For the case that we are called single threaded
  if (mContext.isMaster(&Context))
    {
      context * pIt = mContext.beginThread();
      context * pEnd = mContext.endThread();

      for (; pIt != pEnd; ++pIt)
        if (mContext.isThread(pIt))
          *pIt = Context;
    }
}

template <> 
inline std::vector< CNode * > & CSetCollector< CNode, CNodeElementSelector >::getSet() const
{
  return mpSelector->activeContent().mNodes;
}

template <> 
inline std::vector< CEdge * > & CSetCollector< CEdge, CEdgeElementSelector >::getSet() const
{
  return mpSelector->activeContent().edges;
}

template <> 
inline CNode * CSetCollector< CNode, CNodeElementSelector >::offset() const 
{
  return CNetwork::Context.Active().beginNode();
}

template <> 
inline CEdge * CSetCollector< CEdge, CEdgeElementSelector >::offset() const
{
  return CNetwork::Context.Active().beginEdge();
}

#endif // SRC_SET_CSETCOLLECOTR_H_