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

#ifndef SRC_UTILITIES_CRANDOM_H_
#define SRC_UTILITIES_CRANDOM_H_

#include <random>

#include "utilities/CContext.h"

class CRandom
{
public:
  class CGenerator : public std::mt19937_64
  {
  public:
    std::mt19937_64::result_type operator()();
  };

  typedef std::mt19937_64 generator_t;

  class CNormal : public std::normal_distribution< double >
  {
  public:
    CNormal(double mean = 0.0, double stddev = 1.0);

    double operator()(generator_t & generator);

  private: 
    struct state 
    {
      bool haveValue;
      double value;  
    };

    ::CContext< state > mState;
  };

  typedef generator_t::result_type result_t;
  typedef std::uniform_int_distribution< result_t > uniform_int;
  typedef uniform_int::param_type uniform_int_p;
  typedef std::uniform_real_distribution< double > uniform_real;
  typedef uniform_real::param_type uniform_real_p;
  typedef CNormal normal;
  typedef normal::param_type normal_p;
  typedef std::gamma_distribution<double> gamma;
  typedef std::gamma_distribution<double>::param_type gamma_p;
  
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
