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

#include "Transmission.h"

Transmission::Transmission()
  : Annotation()
  , mId()
  , mpEntryState(NULL)
  , mpExitState(NULL)
  , mpContactState(NULL)
  , mTransmissibility(0)
  , mSusceptibilityFactorOperation()
  , mInfectivityFactorOperation()
  , mValid(false)
{}

Transmission::Transmission(const Transmission & src)
  : Annotation(src)
  , mId(src.mId)
  , mpEntryState(src.mpEntryState)
  , mpExitState(src.mpExitState)
  , mpContactState(src.mpContactState)
  , mTransmissibility(src.mTransmissibility)
  , mSusceptibilityFactorOperation(src.mSusceptibilityFactorOperation)
  , mInfectivityFactorOperation(src.mInfectivityFactorOperation)
  , mValid(false)
{}

// virtual
Transmission::~Transmission()
{}

void Transmission::fromJSON(const json_t * json)
{}

const std::string & Transmission::getId() const
{
  return mId;
}

const bool & Transmission::isValid() const
{
  return mValid;
}
