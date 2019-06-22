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

#include <cstring>
#include <assert.h>

#include "TraitData.h"
#include "Trait.h"

// static
bool TraitData::hasValue(const TraitData::base & data, const TraitData::value & value)
{
  return (data & value.first) == value.second;
}

// static
void TraitData::setValue(TraitData::base & data, const TraitData::value & value)
{
  base Feature(value.first);

  data &= Feature.flip();
  data |= value.second;
}

// static
TraitData::value TraitData::getValue(const TraitData::base & data, const TraitData::base & feature)
{
  return std::make_pair(feature, data & feature);
}

TraitData::TraitData(unsigned long long val)
  : std::bitset< 32 >(val)
{}

TraitData::TraitData(const TraitData & src)
  : std::bitset< 32 >(src)
{}

TraitData::TraitData(const std::bitset< 32 > & bitset)
  : std::bitset< 32 >(bitset)
{}

TraitData::TraitData(const Trait & trait)
  : std::bitset< 32 >(0)
{
  assert(trait.size() == 4);
}

TraitData::~TraitData()
{}

void TraitData::setBits(const size_t & begin, const size_t & beyond)
{

  if (begin >= beyond || beyond >= 32) return;

  for (size_t i = begin; i < beyond; ++i)
    {
      std::bitset< 32 >::set(i);
    }
}

size_t TraitData::firstBit() const
{
  size_t i = 0;

  for (; i < 32; ++i)
    if (std::bitset< 32 >::operator[](i)) break;

  return i;
}
