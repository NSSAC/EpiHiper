// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include "utilities/CRandom.h"

#include "CSimConfig.h"
#include "utilities/CCommunicate.h"

// static
void CRandom::init(size_t seed)
{

  if (seed == -1)
    {
      CRandom::seed(std::random_device()());
      return;
    }

  const size_t & Replicate = CSimConfig::getReplicate();

  if (Replicate == -1)
    {
      CRandom::seed(seed);
      return;
    }

  CRandom::seed(seed + Replicate);
}

// static
void CRandom::seed(CRandom::result_t value)
{
  uint64_t Seed = value;
  CCommunicate::broadcast(&Seed, sizeof(uint64_t), 0);

  CRandom::G.seed(Seed + CCommunicate::MPIRank);
}
