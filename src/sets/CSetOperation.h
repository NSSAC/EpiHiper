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

#ifndef SRC_SETS_CSETOPERATION_H_
#define SRC_SETS_CSETOPERATION_H_

#include "sets/CSetContent.h"

class CSetOperation: public CSetContent
{
public:
  CSetOperation();

  CSetOperation(const CSetOperation & src);

  CSetOperation(const json_t * json);

  virtual ~CSetOperation();

  virtual void fromJSON(const json_t * json);

  virtual void compute();

private:
  void computeUnion();

  void computeIntersection();

  void (CSetOperation::*mpCompute)();

  std::set< CSetContent * > mSets;
};

#endif /* SRC_SETS_CSETOPERATION_H_ */
