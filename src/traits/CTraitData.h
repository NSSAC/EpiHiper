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
