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

#ifndef SRC_UTILITIES_CRANDOM_H_
#define SRC_UTILITIES_CRANDOM_H_

#include <random>

#include "utilities/CContext.h"

class CRandom
{
public:
  class debug_generator : public std::mt19937_64
  {
  public:
    std::mt19937_64::result_type operator()();
  };

  // Change to CGe
  typedef std::mt19937_64 generator_t;
  typedef generator_t::result_type result_t;

  template < class CType > struct DistributionContext
  {
    mutable CType distribution;
    mutable generator_t * pGenerator;
  };

  template < class CType > class Distribution : private ::CContext< DistributionContext< CType > >
  {
    typedef DistributionContext< CType > context_type;
    typedef ::CContext< context_type > base;

  public:
    typedef typename CType::result_type result_type;

    Distribution()
      : base()
    {}

    Distribution(const result_type & a, const result_type & b)
      : base()
    {
      init(a, b);
    }

    void init(const result_type & a, const result_type & b)
    {
      base::init();

      base::Master() = context_type({CType(a, b), &CRandom::G.Master()});

      context_type * pIt = base::beginThread();
      context_type * pEnd = base::endThread();
      generator_t * pG = CRandom::G.beginThread();

      for (; pIt != pEnd; ++pIt, ++pG)
        if (base::isThread(pIt))
          *pIt = context_type({CType(a, b), pG});
    }

    result_type sample() const
    {
      const context_type & Active = base::Active();
      
      return Active.distribution.operator()(*Active.pGenerator);
    }
  };

  typedef std::uniform_int_distribution< result_t > uniform_int;
  typedef std::uniform_real_distribution< double > uniform_real;
  typedef std::normal_distribution< double > normal;
  typedef std::gamma_distribution<double> gamma;
  
  static void seed(result_t seed);
  static void init(size_t seed);
  static result_t getSeed();
  class CContext : public ::CContext< generator_t >
  {
  public:
    generator_t & Active();
  };

  static CContext G;

private:
  static bool mHaveSeed;
  static result_t mSeed;
};

#endif /* SRC_UTILITIES_CRANDOM_H_ */
