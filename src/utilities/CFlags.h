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

#ifndef SRC_UTILITIES_CFLAGS_H_
#define SRC_UTILITIES_CFLAGS_H_

#include <bitset>
#include <vector>

template < class Enum > class CFlags : public std::bitset< static_cast< size_t >(Enum::__SIZE) >
{
public:
  typedef std::bitset< static_cast< size_t >(Enum::__SIZE) > bitset;

  /**
   * Static member where no flag is set
   */
  static const CFlags None;

  /**
   * Static member where all flag is set
   */
  static const CFlags All;

  /**
   * Default constructor
   */
  CFlags():
    bitset()
  {}

  /**
   * Copy constructor
   * @param const CFlags & src
   */
  CFlags(const CFlags & src):
    bitset(src)
  {}

  /**
   * Enum constructor
   * @param const Enum & flag
   */
  CFlags(const Enum & flag):
    bitset()
  {
    bitset::set(static_cast< size_t >(flag));
  }

  /**
   * Bitset constructor
   * @param const bitset & flags
   */
  CFlags(const bitset & flags):
    bitset(flags)
  {}

  /**
   * String constructor
   * @param const std::string & str
   */
  CFlags(const std::string & str):
    bitset(str)
  {}

  /**
   * Destructor
   */
  ~CFlags() {}

  /**
   * Assignment operator
   * @param const CFlags & rhs
   * @return CFlags & *this
   */
  CFlags & operator=(const CFlags & rhs)
  {
    if (this != &rhs)
      this->bitset::operator=(rhs);

    return *this;
  }

  /**
   * Cast to bitset
   */
  operator bitset()
  {
    return *this;
  }

  /**
   * Cast to bool
   */
  operator bool () const
  {
    return operator != (None);
  }

  /**
   * Equality comparison operator
   * @param const CFlags< Enum > & rhs
   * @return bool equal
   */
  bool operator == (const CFlags< Enum > & rhs) const
  {
    return bitset::operator == ((bitset) rhs);
  }

  /**
   * Inequality comparison operator
   * @param const CFlags< Enum > & rhs
   * @return bool notEqual
   */
  bool operator != (const CFlags< Enum > & rhs) const
  {
    return bitset::operator != ((bitset) rhs);
  }

  /**
   * Bitwise OR assignment operator
   * @param const CFlags & flags
   * @return CFlags & *this
   */
  CFlags & operator |= (const CFlags & flags)
  {
    bitset::operator |= ((bitset) flags);

    return *this;
  }

  /**
   * Bitwise OR assignment operator
   * @param const Enum & flag
   * @return CFlags & *this
   */
  CFlags & operator |= (const Enum & flag)
  {
    return operator |= (CFlags(flag));
  }

  /**
   * Bitwise AND assignment operator
   * @param const CFlags & flags
   * @return CFlags & *this
   */
  CFlags & operator &= (const CFlags & flags)
  {
    bitset::operator &= ((bitset) flags);

    return *this;
  }

  /**
   * Bitwise AND assignment operator
   * @param const Enum & flag
   * @return CFlags & *this
   */
  CFlags & operator &= (const Enum & flag)
  {
    return operator &= (CFlags(flag));
  }

  /**
   * Bitwise XOR assignment operator
   * @param const CFlags & flags
   * @return CFlags & *this
   */
  CFlags & operator ^= (const CFlags & flags)
  {
    bitset::operator ^= ((bitset) flags);

    return *this;
  }

  /**
   * Bitwise XOR assignment operator
   * @param const Enum & flag
   * @return CFlags & *this
   */
  CFlags & operator ^= (const Enum & flag)
  {
    return operator ^= (CFlags(flag));
  }

  /**
   * Check whether a the given flag is set
   * @param const Enum & flag
   * @return bool isSet
   */
  bool isSet(const Enum & flag) const
  {
    return bitset::operator[](static_cast< size_t >(flag));
  }
};

/**
 * Bitwise OR operator
 * @param const CFlags< Enum > & lhs
 * @param const CFlags< Enum > & rhs
 * @return CFlags< Enum > result
 */
template < class Enum >
CFlags< Enum > operator | (const CFlags< Enum > & lhs, const CFlags< Enum > & rhs)
{
  return operator | ((std::bitset< static_cast< size_t >(Enum::__SIZE) >) lhs,
                     (std::bitset< static_cast< size_t >(Enum::__SIZE) >) rhs);
}

/**
 * Bitwise OR operator
 * @param const CFlags< Enum > & lhs
 * @param const Enum & rhs
 * @return CFlags< Enum > result
 */
template < class Enum >
CFlags< Enum > operator | (const CFlags< Enum > & lhs, const Enum & rhs)
{
  return operator | (lhs, CFlags< Enum >(rhs));
}

/**
 * Bitwise AND operator
 * @param const CFlags< Enum > & lhs
 * @param const CFlags< Enum > & rhs
 * @return CFlags< Enum > result
 */
template < class Enum >
CFlags< Enum > operator & (const CFlags< Enum > & lhs, const CFlags< Enum > & rhs)
{
  return operator & ((std::bitset< static_cast< size_t >(Enum::__SIZE) >) lhs,
                     (std::bitset< static_cast< size_t >(Enum::__SIZE) >) rhs);
}

/**
 * Bitwise AND operator
 * @param const CFlags< Enum > & lhs
 * @param const Enum & rhs
 * @return CFlags< Enum > result
 */
template < class Enum >
CFlags< Enum > operator & (const CFlags< Enum > & lhs, const Enum & rhs)
{
  return operator & (lhs, CFlags< Enum >(rhs));
}

/**
 * Bitwise XOR operator
 * @param const CFlags< Enum > & lhs
 * @param const CFlags< Enum > & rhs
 * @return CFlags< Enum > result
 */
template < class Enum >
CFlags< Enum > operator ^ (const CFlags< Enum > & lhs, const CFlags< Enum > & rhs)
{
  return operator ^ ((std::bitset< static_cast< size_t >(Enum::__SIZE) >) lhs,
                     (std::bitset< static_cast< size_t >(Enum::__SIZE) >) rhs);
}

/**
 * Bitwise XOR operator
 * @param const CFlags< Enum > & lhs
 * @param const Enum & rhs
 * @return CFlags< Enum > result
 */
template < class Enum >
CFlags< Enum > operator ^ (const CFlags< Enum > & lhs, const Enum & rhs)
{
  return operator ^ (lhs, CFlags< Enum >(rhs));
}

// static
template< class Enum > const CFlags< Enum > CFlags< Enum >::None;

// static
template< class Enum > const CFlags< Enum > CFlags< Enum >::All(~None);

#endif /* SRC_UTILITIES_CFLAGS_H_ */
