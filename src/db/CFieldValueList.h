// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
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
