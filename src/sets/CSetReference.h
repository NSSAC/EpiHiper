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

#ifndef SRC_SETS_CSETREFERENCE_H_
#define SRC_SETS_CSETREFERENCE_H_

#include "sets/CSetContent.h"

struct json_t;
class CSet;

class CSetReference : public CSetContent
{
public:
  CSetReference();

  CSetReference(const CSetReference & src);

  CSetReference(const json_t * json);

  virtual ~CSetReference();

  virtual void fromJSON(const json_t * json);

  virtual void compute();

  virtual bool contains(CNode * pNode) const;

  virtual bool contains(CEdge * pEdge) const;

  virtual bool contains(const CValueInterface & value) const;

  virtual std::set< CEdge * >::const_iterator beginEdges() const;

  virtual std::set< CEdge * >::const_iterator endEdges() const;

  virtual std::set< CNode * >::const_iterator beginNodes() const;

  virtual std::set< CNode * >::const_iterator endNodes() const;

  virtual const std::set< CEdge * > & getEdges() const;

  virtual const std::set< CNode * > & getNodes() const;

  virtual const std::map< CValueList::Type, CValueList > & getDBFieldValues() const;

  virtual std::set< CEdge * > & getEdges();

  virtual std::set< CNode * > & getNodes();

  virtual std::map< CValueList::Type, CValueList > & getDBFieldValues();

private:
  std::string mIdRef;
  CSet * mpSet;
};

#endif /* SRC_SETS_CSETREFERENCE_H_ */
