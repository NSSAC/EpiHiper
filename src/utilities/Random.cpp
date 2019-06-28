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

#include "utilities/Random.h"
#include "Communicate.h"

// static
Random::generator_t Random::G;

// static
void Random::randomSeed()
{
  std::random_device rd;
  seed(rd());
}

// static
void Random::seed(Random::result_t value)
{
  uint64_t Seed = value;
  Communicate::broadcast(&Seed, 1, MPI_UINT64_T, 0);

  Random::G.seed(Seed + Communicate::Rank);
}
