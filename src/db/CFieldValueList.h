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

/*
 * CFieldValueList.h
 *
 *  Created on: Aug 8, 2019
 *      Author: shoops
 */

#ifndef SRC_DB_CFIELDVALUELIST_H_
#define SRC_DB_CFIELDVALUELIST_H_

#include "math/CValueList.h"

class CFieldValue;

class CFieldValueList: public CValueList
{
public:
  CFieldValueList(const Type & type = Type::boolean);

  CFieldValueList(const CFieldValueList & src);

  CFieldValueList(const json_t * json, const Type & hint);

  virtual ~CFieldValueList();

  bool append(const CFieldValue & value);

  bool append(const CValue & value) = delete;

  virtual void fromJSON(const json_t * json) override;
};

#endif /* SRC_DB_CFIELDVALUELIST_H_ */
