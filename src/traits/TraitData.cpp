#include <cstring>
#include <assert.h>

#include "TraitData.h"
#include "Trait.h"

TraitData::TraitData(unsigned long long val)
  : std::bitset< 32 >(val)
{}

TraitData::TraitData(const TraitData & src)
  : std::bitset< 32 >(src)
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
