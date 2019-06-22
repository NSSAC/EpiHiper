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

#ifndef SRC_TRAITS_TRAITDATA_H_
#define SRC_TRAITS_TRAITDATA_H_

#include <cstddef>
#include <bitset>

class Trait;

class TraitData: public std::bitset< 32 >
{
public:
  typedef std::bitset< 32 > base;

  TraitData(unsigned long long val = 0);

  TraitData(const TraitData & src);

  TraitData(const base & bitset);

  TraitData(const Trait & trait);

  ~TraitData();

  void setBits(const size_t & begin, const size_t & beyond);

  size_t firstBit() const;

  static void setValue(base & data, base feature, const base & value);

  static base getValue(const base & data, const base & feature);
};

#endif /* SRC_TRAITS_TRAITDATA_H_ */
