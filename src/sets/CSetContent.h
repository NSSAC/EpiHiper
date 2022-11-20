// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
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

class CNode;
class CEdge;

class CSetContent: public CComputable
{
public:
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
  static CSetContent * create(const json_t * json);
  // static CSetContent * copy(const CSetContent * pSetContent);
  static void destroy(CSetContent *& pSetContent);

protected:
  CSetContent();

  CSetContent(const CSetContent & src);

public:
  virtual ~CSetContent();

  virtual CSetContent * copy() const = 0;

  virtual bool computeProtected() override;

  virtual void fromJSON(const json_t * json) = 0;

  const bool & isValid() const;

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

private:
  CContext< SetContent > mContext;

protected:
  bool mValid;
};

#endif /* SRC_INTERVENTION_CSETCONTENT_H_ */
