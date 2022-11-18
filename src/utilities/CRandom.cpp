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

#include "utilities/CRandom.h"

#include "CSimConfig.h"
#include "utilities/CCommunicate.h"

CRandom::result_t CRandom::CGenerator::operator()()
{
  result_t r = std::mt19937_64::operator()();

  CLogger::trace() << "CRandom::CGenerator::operator " << r;

  return r;
}

CRandom::CNormal::CNormal(double mean, double stddev)
  : std::normal_distribution< double >(mean, stddev)
  , mState()
{
  mState.init();

  state * pIt = mState.beginThread();
  state * pEnd = mState.endThread();

  for (; pIt != pEnd; ++pIt)
    {
      pIt->haveValue = false;
      pIt->value = 0.0;
    }
}

double CRandom::CNormal::operator()(generator_t & generator)
{
  state & State = mState.Active();

  State.haveValue = !State.haveValue;

  if (!State.haveValue)
    return State.value;

  double Value;

#pragma omp critical
  {
    Value = std::normal_distribution< double >::operator()(generator);
    State.value = std::normal_distribution< double >::operator()(generator);
  }

  return Value;
}



CRandom::generator_t & CRandom::CContext::Active()
{
  // Master is not initialized and must not be used
  return ThreadData[omp_get_thread_num()];
}

// static 
bool CRandom::mHaveSeed = false;

// static 
CRandom::result_t CRandom::mSeed = -1;

// static
void CRandom::init(size_t seed)
{
  CRandom::G.init();

  mHaveSeed = (seed != std::numeric_limits< size_t >::max());

  if (!mHaveSeed)
    {
      CRandom::seed(std::random_device()());
      return;
    }

  const size_t & Replicate = CSimConfig::getReplicate();

  if (Replicate == std::numeric_limits< size_t >::max())
    {
      CRandom::seed(seed);
      return;
    }

  CRandom::seed(seed + Replicate);
}

// static
void CRandom::seed(CRandom::result_t value)
{
  mSeed = value;

  int TotalSeeds = CCommunicate::TotalProcesses();
  result_t Seeds[TotalSeeds];
  result_t *pSeed = Seeds;
  result_t *pSeedEnd = pSeed + TotalSeeds;

  if (CCommunicate::GlobalThreadIndex() == 0)
    {
      std::seed_seq seq{value, value + 1, value + 3, value + 5, value + 7};
      seq.generate(pSeed, pSeedEnd);
    }

  CCommunicate::broadcast(Seeds, sizeof(result_t) * TotalSeeds, 0);

  generator_t * pIt = G.beginThread();  
  generator_t * pEnd = G.endThread();

  for (; pIt != pEnd; ++pIt)
    {
      if (mHaveSeed)
        CLogger::debug() << "CRandom::seed: Seeding thread " << G.globalIndex(pIt) << " with: " << Seeds[G.globalIndex(pIt)];

      pIt->seed(Seeds[G.globalIndex(pIt)]);
    }  
}

// static 
CRandom::result_t CRandom::getSeed()
{
  return mSeed;
}