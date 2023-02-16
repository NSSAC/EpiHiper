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
 * CStatus.h
 *
 *  Created on: Sep 4, 2019
 *      Author: shoops
 */

#ifndef SRC_UTILITIES_CSTATUS_H_
#define SRC_UTILITIES_CSTATUS_H_

#include <string>
#include <limits>

struct json_t;

class CStatus
{
public:
  static void load(const std::string & id, const std::string & name, const std::string &file);

  static void update(const std::string & status, const double & progress = std::numeric_limits< double >::quiet_NaN());

  static void success();

private:
  static json_t * pJSON;
  static json_t * pId;
  static json_t * pName;
  static json_t * pStatus;
  static json_t * pDetail;
  static json_t * pProgress;
  static std::string fileName;
};

#endif /* SRC_UTILITIES_CSTATUS_H_ */
