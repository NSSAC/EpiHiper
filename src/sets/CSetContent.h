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

#ifndef SRC_INTERVENTION_CSETCONTENT_H_
#define SRC_INTERVENTION_CSETCONTENT_H_

#include "math/CValueList.h"
#include "math/CComputable.h"
#include "utilities/CContext.h"
#include "utilities/CLogger.h"

#include <memory>
#include <set>

class CNetwork;
class CNode;
class CEdge;

class CSetContent: public CComputable
{
public:
  enum struct Scope
  {
    global,
    local
  };

  typedef std::shared_ptr< CSetContent > shared_pointer;

  class CDBFieldValues : public std::map< CValueList::Type, CValueList >
  {
  public:
    size_t size() const;
  };

public:

  struct SetContent
  {
    void clear();

    void sync();

    class Nodes
    {
    public:
      friend void SetContent::clear();
      friend void SetContent::sync();

      std::vector< CNode * >::const_iterator begin() const
      {
        return mBegin;
      }

      std::vector< CNode * >::const_iterator end() const
      {
        return mEnd;
      }

      size_t size() const
      {
        return mSize;
      }

    private:
      std::vector< CNode * >::const_iterator mBegin;
      std::vector< CNode * >::const_iterator mEnd;
      size_t mSize = 0;
    };

    Nodes & nodes(const Scope & scope);

    std::vector< CNode * > mNodes;
    Nodes globalNodes;
    Nodes localNodes;
    std::vector< CEdge * > edges;
    CDBFieldValues dBFieldValues;
  };

  typedef SetContent::Nodes NodeContent;

  static shared_pointer create(const json_t * json);

  struct Compare
  {
    bool operator()(const shared_pointer & lhs, const shared_pointer & rhs) const;
  };

  static std::set< shared_pointer, Compare > Unique;

  template< class function_pointer >
  static bool pointerLessThan(const function_pointer pLeft, const function_pointer pRight)
  {
    char Left[19];
    sprintf(Left, "%p", pLeft);

    char Right[19];
    sprintf(Right, "%p", pRight);

    return strcmp(Left, Right) < 0;
  }

  enum struct FilterType {
    edge,
    node,
    none
  };

  template < class element_type >
  class Filter {
    CSetContent * mpSetContent;
    std::vector< element_type * > * mpSet;

  public:
    Filter(CSetContent * pSetContent);

    void addMatching(element_type * pType);

    void start();

    void finish();
  };

protected:
  enum struct Type {
    sampled,
    edgeElementSelector,
    nodeElementSelector,
    dbFieldSelector,
    set,
    setReference,
    operation,
    __SIZE
  };

  template< class element_type >
  static int comparePointer(const element_type * pLhs, const element_type * pRhs);

  CSetContent(const Type & type = Type::__SIZE);

  CSetContent(const CSetContent & src);

public:
  virtual ~CSetContent();

  virtual FilterType filterType() const;

  virtual bool filter(const CNode * pNode) const;
 
  virtual bool filter(const CEdge * pEdge) const;
 
  bool lessThan(const CSetContent & rhs) const;

  void fromJSON(const json_t * json);

  const std::string & getJSON() const;

  bool contains(CNode * pNode) const;

  bool contains(CEdge * pEdge) const;

  bool contains(const CValueInterface & value) const;

  const SetContent & activeContent() const;

  SetContent & activeContent();

  virtual const SetContent::Nodes & getNodeContent(const Scope & scope) const;

  inline virtual const CContext< SetContent > & getContext() const
  {
    return mContext;
  }

  inline virtual CContext< SetContent > & getContext()
  {
    return mContext;
  }

  void sampleMax(const size_t & max, CSetContent & sampled, CSetContent & NotSampled) const;

  void samplePercent(const double & percent, CSetContent & sampled, CSetContent & NotSampled) const;

  size_t size() const;

  size_t totalSize() const;

  CContext< size_t > sizes() const;

  void setScope(const Scope & scope);

  const Scope & getScope() const;

protected:
  virtual bool computeProtected() override;

  virtual bool computeSetContent() = 0;

  virtual void fromJSONProtected(const json_t * json) = 0;

  virtual bool lessThanProtected(const CSetContent & rhs) const = 0;

  virtual void setScopeProtected() = 0;

private:
  static std::set< shared_pointer > SetContents;

  CContext< SetContent > mContext;

protected:
  Type mType;
  
  std::string mJSON;

  Scope mScope;
};

template < class element_type >
int CSetContent::comparePointer(const element_type * pLhs, const element_type * pRhs)
{
  if (pLhs != NULL
      && pRhs != NULL)
    {
      if (*pLhs != *pRhs)
        return *pLhs < *pRhs ? -1 : 1;
      else
        return 0;
    }

  if (pLhs != pRhs)
    return pLhs < pRhs ? -1 : 1;

  return 0;
}

template <>
inline CSetContent::Filter< CEdge >::Filter(CSetContent * pSetContent)
  : mpSetContent(pSetContent)
  , mpSet(&mpSetContent->activeContent().edges)
{}

template <>
inline CSetContent::Filter< CNode >::Filter(CSetContent * pSetContent)
  : mpSetContent(pSetContent)
  , mpSet(&mpSetContent->activeContent().mNodes)
{}

template < class element_type >
void CSetContent::Filter< element_type >::addMatching(element_type * pType)
{
  if (mpSetContent->filter(pType))
    mpSet->push_back(pType);
}

template < class element_type >
void CSetContent::Filter< element_type >::start() 
{
  mpSetContent->activeContent().clear();
}

template < class element_type >
void CSetContent::Filter< element_type >::finish() 
{
  mpSetContent->activeContent().sync();
  CLogger::debug("CSetContent: '{}' contains '{}' elements.", mpSetContent->getComputableId(), mpSet->size());
}

#endif /* SRC_INTERVENTION_CSETCONTENT_H_ */
