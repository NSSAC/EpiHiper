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

#ifndef SRC_INTERVENTION_CSETCONTENT_H_
#define SRC_INTERVENTION_CSETCONTENT_H_

#include "math/CValueList.h"
#include "math/CComputable.h"
#include "utilities/CContext.h"

#include <memory>
#include <set>

class CNode;
class CEdge;

class CSetContent: public CComputable
{
public:
  typedef std::shared_ptr< CSetContent > CSetContentPtr;

  class CDBFieldValues : public std::map< CValueList::Type, CValueList >
  {
  public:
    size_t size() const;
  };

 struct SetContent
  {
    std::vector< CNode * > nodes;
    std::vector< CEdge * > edges;
    CDBFieldValues dBFieldValues;
  };

public:
  static CSetContentPtr create(const json_t * json);

  struct Compare
  {
    bool operator()(const CSetContentPtr & lhs, const CSetContentPtr & rhs);
  };

  static std::set< CSetContentPtr, Compare > UniqueSetContent;

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

  bool lessThan(const CSetContent & rhs) const;

  void fromJSON(const json_t * json);

  bool contains(CNode * pNode) const;

  bool contains(CEdge * pEdge) const;

  bool contains(const CValueInterface & value) const;

  std::vector< CEdge * >::const_iterator beginEdges() const;

  std::vector< CEdge * >::const_iterator endEdges() const;

  std::vector< CNode * >::const_iterator beginNodes() const;

  std::vector< CNode * >::const_iterator endNodes() const;

  const std::vector< CEdge * > & getEdges() const;

  const std::vector< CNode * > & getNodes() const;

  const CDBFieldValues & getDBFieldValues() const;

  std::vector< CEdge * > & getEdges();

  std::vector< CNode * > & getNodes();

  CDBFieldValues & getDBFieldValues();

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

protected:
  virtual bool computeProtected() override;

  virtual void fromJSONProtected(const json_t * json) = 0;

  virtual bool lessThanProtected(const CSetContent & rhs) const = 0;

private:
  static std::set< CSetContentPtr > SetContents;

  Type mType;
  
  CContext< SetContent > mContext;

  std::string mJSON;
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

#endif /* SRC_INTERVENTION_CSETCONTENT_H_ */
