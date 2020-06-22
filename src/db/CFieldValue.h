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
 * CFieldValue.h
 *
 *  Created on: Aug 8, 2019
 *      Author: shoops
 */

#ifndef SRC_DB_CFIELDVALUE_H_
#define SRC_DB_CFIELDVALUE_H_

#include "math/CValue.h"

class CFieldValue: public CValue
{
public:
  CFieldValue() = delete;

  CFieldValue(const size_t & id);

  CFieldValue(const double & number);

  CFieldValue(const int & integer);

  CFieldValue(const std::string & str);

  CFieldValue(const json_t * json, const Type & hint);

  virtual ~CFieldValue();

  virtual void fromJSON(const json_t * json) override;
};

#endif /* SRC_DB_CFIELDVALUE_H_ */
