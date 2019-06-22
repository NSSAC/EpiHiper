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

#include "Progression.h"

Progression::Progression()
  : Annotation()
  , mId()
  , mpEntryState(NULL)
  , mpExitState(NULL)
  , mProbability(0)
  , mDwellTime()
  , mSusceptibilityFactorOperation()
  , mInfectivityFactorOperation()
  , mValid(false)
{}

Progression::Progression(const Progression & src)
  : Annotation(src)
  , mId(src.mId)
  , mpEntryState(src.mpEntryState)
  , mpExitState(src.mpExitState)
  , mProbability(src.mProbability)
  , mDwellTime(src.mDwellTime)
  , mSusceptibilityFactorOperation(src.mSusceptibilityFactorOperation)
  , mInfectivityFactorOperation(src.mInfectivityFactorOperation)
  , mValid(false)
{}

// virtual
Progression::~Progression()
{}

void Progression::fromJSON(const json_t * json)
{}

const std::string & Progression::getId() const
{
  return mId;
}

const bool & Progression::isValid() const
{
  return mValid;
}
