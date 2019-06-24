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

#include <jansson.h>

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
{
  mValid = true;

  json_t * pValue = json_object_get(json, "states");

  for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i)
    {
      State State;
      State.fromJSON(json_array_get(pValue, i));

      if (State.isValid())
        {
          mStates[State.getId()] = State;
        }
      else
        {
          mValid = false;
        }
    }

  pValue = json_object_get(json, "initialState");

  if (json_is_string(pValue))
    {
      std::map< std::string, State>::const_iterator found = mStates.find(json_string_value(pValue));

      if (found != mStates.end())
        {
          mpInitialState = &found->second;
        }
    }

  mValid &= (mpInitialState != NULL);

  pValue = json_object_get(json, "transmissions");

  mTransmissions.resize(json_array_size(pValue));
  std::vector< Transmission >::iterator itTrans = mTransmissions.begin();
  std::vector< Transmission >::iterator endTrans = mTransmissions.end();

  for (size_t i = 0; itTrans != endTrans; ++itTrans, ++i)
    {
      itTrans->fromJSON(json_array_get(pValue, i), mStates);
      mValid &= itTrans->isValid();
    }

  pValue = json_object_get(json, "transitions");

  mProgressions.resize(json_array_size(pValue));
  std::vector< Progression >::iterator itProg = mProgressions.begin();
  std::vector< Progression >::iterator endProg = mProgressions.end();

  for (size_t i = 0; itProg != endProg; ++itProg, ++i)
    {
      itProg->fromJSON(json_array_get(pValue, i), mStates);
      mValid &= itProg->isValid();
    }

  Annotation::fromJSON(json);
}

const bool & Model::isValid() const
{
  return mValid;
}
