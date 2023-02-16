// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2023 Rector and Visitors of the University of Virginia 
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

/*
 * CSizeOf.h
 *
 *  Created on: Sep 2, 2019
 *      Author: shoops
 */

#ifndef SRC_MATH_CSIZEOF_H_
#define SRC_MATH_CSIZEOF_H_

#include <vector>

#include "math/CValue.h"
#include "math/CComputable.h"
#include "utilities/CCommunicate.h"

class CSetContent;
struct json_t;

class CSizeOf : public CValue, public CComputable
{
public:
  // static
  std::vector< CSizeOf * > GetInstances();

  CSizeOf();

  CSizeOf(const CSizeOf & src);

  CSizeOf(const json_t * json);

  virtual ~CSizeOf();

  virtual CValueInterface * copy() const override;

  virtual bool computeProtected() override;

  virtual void fromJSON(const json_t * json) override;

private:
  static std::vector< CSizeOf * > INSTANCES;

  int broadcastSize();

  CCommunicate::ErrorCode receiveSize(std::istream & is, int sender);

  CSetContent * mpSetContent;
  size_t mIndex;
  std::string mIdentifier;
};

#endif /* SRC_MATH_CSIZEOF_H_ */
