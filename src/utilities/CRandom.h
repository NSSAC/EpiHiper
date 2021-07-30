// BEGIN: Copyright 
// Copyright (C) 2019 - 2021 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

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

  class CContext : public ::CContext< generator_t >
  {
  public:
    generator_t & Active();
  };

  static CContext G;

private:
  static bool haveSeed;
};

#endif /* SRC_UTILITIES_CRANDOM_H_ */
