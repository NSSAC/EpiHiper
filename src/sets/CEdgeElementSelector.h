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

#ifndef SRC_SETS_CEDGEELEMENTSELECTOR_H_
#define SRC_SETS_CEDGEELEMENTSELECTOR_H_

#include "../math/CEdgeProperty.h"
#include "math/CValueList.h"
#include "sets/CSetContent.h"

class CEdgeElementSelector: public CSetContent
{
public:
  CEdgeElementSelector();

  CEdgeElementSelector(const CEdgeElementSelector & src);

  CEdgeElementSelector(const json_t * json);

  virtual ~CEdgeElementSelector();

private:
  CEdgeProperty mLeft;
  CSetContent * mpSetContent;
  CValueList mValueList;
};

#endif /* SRC_SETS_CEDGEELEMENTSELECTOR_H_ */
