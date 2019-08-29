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

#ifndef SRC_SETS_CDBFIELDSELECTOR_H_
#define SRC_SETS_CDBFIELDSELECTOR_H_

#include "sets/CSetContent.h"
#include "math/CValue.h"

class CDBFieldSelector : public CSetContent
{
public:
  CDBFieldSelector();

  CDBFieldSelector(const CDBFieldSelector & src);

  CDBFieldSelector(const json_t * json);

  virtual ~CDBFieldSelector();

  virtual void fromJSON(const json_t * json);

  virtual void compute();

private:
  std::string mTable;
  std::string mField;
  CValue::Type mFieldType;
  CSetContent * mpSelector;
};

#endif /* SRC_SETS_CDBFIELDSELECTOR_H_ */
