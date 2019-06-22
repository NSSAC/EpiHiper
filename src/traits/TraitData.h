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
  TraitData(unsigned long long val = 0);

  TraitData(const TraitData & src);

  TraitData(const Trait & trait);

  ~TraitData();

  void setBits(const size_t & begin, const size_t & beyond);

  size_t firstBit() const;
};

#endif /* SRC_TRAITS_TRAITDATA_H_ */
