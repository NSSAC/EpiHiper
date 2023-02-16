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
 * CVariableList.h
 *
 *  Created on: Aug 4, 2019
 *      Author: shoops
 */

#ifndef SRC_VARIABLES_CVARIABLELIST_H_
#define SRC_VARIABLES_CVARIABLELIST_H_

#include <vector>
#include <map>

#include "variables/CVariable.h"

struct json_t;

class CVariableList: protected std::vector< CVariable * >
{
private: 
  typedef std::vector< CVariable * > base;

public:
  typedef std::vector< CVariable * >::const_iterator const_iterator;

  static CVariableList INSTANCE;

  CVariableList();

  CVariableList(const CVariableList & src);

  virtual ~CVariableList();

  virtual void fromJSON(const json_t * json);

  void toBinary(std::ostream & os) const;

  void fromBinary(std::istream & is);

  CVariable & operator[](const size_t & id);

  CVariable & operator[](const std::string & id);

  CVariable & operator[](const json_t * json);

  void resetAll(const bool & force = false);

  bool append(CVariable * pVariable);

  const_iterator begin() const;
  
  const_iterator end() const;

private:
  std::map< std::string, size_t > mId2Index;
  bool mValid;
};

#endif /* SRC_VARIABLES_CVARIABLELIST_H_ */
