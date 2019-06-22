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

#include "State.h"

State::State()
  : Annotation()
  , mId()
  , mSusceptibility(0)
  , mInfectivity(0)
  , mValid(false)
{}

State::State(const State & src)
  : Annotation(src)
  , mId(src.mId)
  , mSusceptibility(src.mSusceptibility)
  , mInfectivity(src.mInfectivity)
  , mValid(src.mValid)
{}

// virtual
State::~State()
{}

void State::fromJSON(const json_t * json)
{}

const std::string & State::getId() const
{
  return mId;
}

const bool & State::isValid() const
{
  return mValid;
}
