// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
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
 * TraitData.h
 *
 *  Created on: Jun 21, 2019
 *      Author: shoops
 */

#ifndef SRC_TRAITS_CTRAITDATA_H_
#define SRC_TRAITS_CTRAITDATA_H_

#include <cstddef>
#include <bitset>
#include <utility>

class CTrait;

class CTraitData: public std::bitset< 32 >
{
public:
  typedef unsigned int base;

  typedef std::pair< base, base > value;

  CTraitData(base val = 0);

  CTraitData(const CTraitData & src);

  CTraitData(const CTrait & trait);

  ~CTraitData();

  void setBits(const size_t & begin, const size_t & beyond);

  size_t firstBit() const;

  static void setValue(base & data, const value & value);

  static value getValue(const base & data, const base & feature);

  static bool hasValue(const base & data, const value & value);

};

#endif /* SRC_TRAITS_CTRAITDATA_H_ */
