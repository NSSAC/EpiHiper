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

/*
 * CSizeOf.h
 *
 *  Created on: Sep 2, 2019
 *      Author: shoops
 */

#ifndef SRC_MATH_CSIZEOF_H_
#define SRC_MATH_CSIZEOF_H_

#include "math/CValue.h"
#include "math/CComputable.h"

class CSetContent;
struct json_t;

class CSizeOf : public CValue, public CComputable
{
public:
  CSizeOf();

  CSizeOf(const CSizeOf & src);

  CSizeOf(const json_t * json);

  virtual ~CSizeOf();

  virtual void compute();

  void fromJSON(const json_t * json);

private:
  CSetContent * mpSetContent;
  bool mValid;
};

#endif /* SRC_MATH_CSIZEOF_H_ */