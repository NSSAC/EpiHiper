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

#ifndef SRC_UTILITIES_CRANDOM_H_
#define SRC_UTILITIES_CRANDOM_H_

#include <random>

class CRandom
{
public:
  typedef std::mt19937_64 generator_t;
  typedef generator_t::result_type result_t;
  typedef std::uniform_int_distribution< result_t > uniform_int;
  typedef uniform_int::param_type uniform_int_p;
  typedef std::uniform_real_distribution< double > uniform_real;
  typedef uniform_real::param_type uniform_real_p;
  typedef std::normal_distribution< double > normal;
  typedef normal::param_type normal_p;

  static generator_t G;

  static void seed(result_t seed);
  static void init();
};

#endif /* SRC_UTILITIES_CRANDOM_H_ */
