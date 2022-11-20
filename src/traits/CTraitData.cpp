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

#include <cstring>
#include <assert.h>
#include <iostream>

#include "traits/CTrait.h"
#include "traits/CTraitData.h"

// static
bool CTraitData::hasValue(const CTraitData::base & data, const CTraitData::value & value)
{
  return (data & value.first) == value.second;
}

// static
void CTraitData::setValue(CTraitData::base & data, const CTraitData::value & value)
{
  base F(value.first);

  data &= ~F;
  data |= value.second;
}

// static
CTraitData::value CTraitData::getValue(const CTraitData::base & data, const CTraitData::base & feature)
{
  return value(feature, data & feature);
}

CTraitData::CTraitData(CTraitData::base val)
  : std::bitset< 32 >((unsigned long long) val)
{}

CTraitData::CTraitData(const CTraitData & src)
  : std::bitset< 32 >(src)
{}

CTraitData::CTraitData(const CTrait & trait)
  : std::bitset< 32 >(0)
{
  assert(trait.size() == 4);
}

CTraitData::~CTraitData()
{}

void CTraitData::setBits(const size_t & begin, const size_t & beyond)
{

  if (begin >= beyond || beyond >= 32) return;

  for (size_t i = begin; i < beyond; ++i)
    {
      std::bitset< 32 >::set(i);
    }
}

size_t CTraitData::firstBit() const
{
  size_t i = 0;

  for (; i < 32; ++i)
    if (std::bitset< 32 >::operator[](i)) break;

  return i;
}
