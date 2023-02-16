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
 * CQuery.h
 *
 *  Created on: Aug 8, 2019
 *      Author: shoops
 */

#ifndef SRC_DB_CQUERY_H_
#define SRC_DB_CQUERY_H_

#include <string>

#include "utilities/CContext.h"

class CValue;
class CValueList;
class CFieldValueList;

struct CQuery
{
public:
  static bool all(const std::string & table,
                   const std::string & resultField,
                   CFieldValueList & result,
                   const bool & local);

  static bool in(const std::string & table,
                   const std::string & resultField,
                   CFieldValueList & result,
                   const bool & local,
                   const std::string & constraintField,
                   const CValueList & constraint,
                   const bool & in = true);

  static bool notIn(const std::string & table,
                   const std::string & resultField,
                   CFieldValueList & result,
                   const bool & local,
                   const std::string & constraintField,
                   const CValueList & constraint);

  static bool where(const std::string & table,
                   const std::string & resultField,
                   CFieldValueList & result,
                   const bool & local,
                   const std::string & constraintField,
                   const CValue & constraint,
                   const std::string & cmp);

private:
  static CContext< std::string > LocalConstraint;
  static size_t Limit;
  static void init();
  static std::string limit(size_t & offset);
};

#endif /* SRC_DB_CQUERY_H_ */
