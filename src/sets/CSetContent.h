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
