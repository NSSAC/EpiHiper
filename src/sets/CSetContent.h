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

#ifndef SRC_INTERVENTION_CSETCONTENT_H_
#define SRC_INTERVENTION_CSETCONTENT_H_

#include "math/CValueList.h"
#include "math/CComputable.h"

class CNode;
class CEdge;

class CSetContent: public CComputable
{
public:
  static CSetContent * create(const json_t * json);
  static CSetContent * copy(const CSetContent * pSetContent);
  static void destroy(CSetContent *& pSetContent);

  CSetContent();

  CSetContent(const CSetContent & src);

  virtual ~CSetContent();

  virtual void fromJSON(const json_t * json);

  const bool & isValid() const;

  bool contains(CNode * pNode) const;

  bool contains(CEdge * pEdge) const;

  bool contains(const CValueInterface & value) const;

  std::set< CEdge * >::const_iterator beginEdges() const;

  std::set< CEdge * >::const_iterator endEdges() const;

  std::set< CNode * >::const_iterator beginNodes() const;

  std::set< CNode * >::const_iterator endNodes() const;

protected:
  std::set< CNode * > mNodes;
  std::set< CEdge * > mEdges;
  std::map< CValueList::Type, CValueList > mDBFieldValues;
  bool mValid;
};

#endif /* SRC_INTERVENTION_CSETCONTENT_H_ */
