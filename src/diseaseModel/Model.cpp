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
#include "Transmission.h"
#include "Progression.h"
#include "Model.h"

Model::Model()
  : Annotation()
  , mStates()
  , mpInitialState(NULL)
  , mTransmissions()
  , mProgressions()
  , mValid(false)
{}

Model::Model(const Model & src)
  : Annotation(src)
  , mStates(src.mStates)
  , mpInitialState(src.mpInitialState)
  , mTransmissions(src.mTransmissions)
  , mProgressions(src.mProgressions)
  , mValid(src.mValid)
{}

// virtual
Model::~Model()
{}

void Model::fromJSON(const json_t * json)
{}

const bool & Model::isValid() const
{
  return mValid;
}
