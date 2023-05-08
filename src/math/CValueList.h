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

#ifndef SRC_MATH_CVALUELIST_H_
#define SRC_MATH_CVALUELIST_H_

#include <set>

#include "math/CValue.h"

class CValueList : public std::set< CValue >
{
public:
  typedef CValueInterface::Type Type;
  typedef std::set< CValue >::iterator iterator;
  typedef std::set< CValue >::const_iterator const_iterator;
  typedef std::set< CValue >::value_type value_type;

  CValueList(const Type & type = Type::number);

  CValueList(const CValueList & src);

  CValueList(const json_t * json);

  CValueList(std::istream & is);

  bool append(const CValue & value);

  bool contains(const CValueInterface & value) const;

  virtual ~CValueList();

  const_iterator begin() const;

  const_iterator end() const;

  const Type & getType() const;

  size_t size() const;

  const bool & isValid() const;

  virtual void fromJSON(const json_t * json);

  void toBinary(std::ostream & os) const;

  void fromBinary(std::istream & is);

  iterator insert (iterator position, const value_type & val);

  template < typename _InputIterator > void insert(_InputIterator __first, _InputIterator __last);

protected:
  Type mType;
  bool mValid;
};

template < typename _InputIterator > void CValueList::insert(_InputIterator __first, _InputIterator __last)
{
  std::set< CValue >::insert(__first, __last);
}


#endif /* SRC_MATH_CVALUELIST_H_ */
