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
 * CValueInterface.h
 *
 *  Created on: Aug 7, 2019
 *      Author: shoops
 */

#ifndef SRC_MATH_CVALUEINTERFACE_H_
#define SRC_MATH_CVALUEINTERFACE_H_

#include <iostream>

#include "traits/CTraitData.h"
#include "diseaseModel/CModel.h"

class CValueInterface
{
  friend std::ostream & operator << (std::ostream & os, const CValueInterface & p);

  friend bool operator<(const CValueInterface & lhs, const CValueInterface & rhs);
  friend bool operator<=(const CValueInterface & lhs, const CValueInterface & rhs);
  friend bool operator>(const CValueInterface & lhs, const CValueInterface & rhs);
  friend bool operator>=(const CValueInterface & lhs, const CValueInterface & rhs);
  friend bool operator==(const CValueInterface & lhs, const CValueInterface & rhs);
  friend bool operator!=(const CValueInterface & lhs, const CValueInterface & rhs);

public:
  typedef bool (*pComparison)(const CValueInterface &, const CValueInterface &);
  typedef void (*pOperator)(double &, const double &);

  enum struct Type
  {
    boolean,
    number,
    integer,
    traitData,
    traitValue,
    string,
    id
  };

  CValueInterface() = delete;

  CValueInterface(const Type & type, void * pValue);

  CValueInterface(const CValueInterface & src);

  CValueInterface(bool & boolean);

  CValueInterface(double & number);

  CValueInterface(int & integer);

  CValueInterface(size_t & id);

  CValueInterface(CTraitData::base & traitData);

  CValueInterface(CTraitData::value & traitValue);

  CValueInterface(std::string & str);

  virtual ~CValueInterface();

  virtual CValueInterface * copy() const;

  const bool & toBoolean() const;

  const double & toNumber() const;

  const int & toInteger() const;

  const size_t & toId() const;

  const CTraitData::base & toTraitData() const;

  const CTraitData::value & toTraitValue() const;

  const std::string & toString() const;

  const Type & getType() const;

  void toBinary(std::ostream & os) const;

  void fromBinary(std::istream & is);

  CValueInterface & operator=(const CValueInterface & rhs);

  static void equal(double & lhs, const double & rhs);

  static void plus(double & lhs, const double & rhs);

  static void minus(double & lhs, const double & rhs);

  static void multiply(double & lhs, const double & rhs);

  static void divide(double & lhs, const double & rhs);

  static bool compatible(const CValueInterface::Type & lhs, const CValueInterface::Type & rhs);
  
  static std::string operatorToString(CValueInterface::pOperator pOperator);
  
protected:
  Type mType;
  void * mpValue;
};

bool operator<(const CValueInterface & lhs, const CValueInterface & rhs);
bool operator<=(const CValueInterface & lhs, const CValueInterface & rhs);
bool operator>(const CValueInterface & lhs, const CValueInterface & rhs);
bool operator>=(const CValueInterface & lhs, const CValueInterface & rhs);
bool operator==(const CValueInterface & lhs, const CValueInterface & rhs);
bool operator!=(const CValueInterface & lhs, const CValueInterface & rhs);

#endif /* SRC_MATH_CVALUEINTERFACE_H_ */
