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

void Progression::fromJSON(const json_t * json, const std::map< std::string, State * > & states)
{
  mValid = true;

  std::map< std::string, State * >::const_iterator found;
  std::map< std::string, State * >::const_iterator notFound = states.end();

  json_t * pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
      mId = json_string_value(pValue);
      mAnnId = mId;
    }

  mValid &= !mId.empty();

  pValue = json_object_get(json, "entryState");

  if (json_is_string(pValue) &&
      (found = states.find(json_string_value(pValue))) != notFound)
    {
      mpEntryState = found->second;
    }

  mValid &= (mpEntryState != NULL);

  pValue = json_object_get(json, "exitState");

  if (json_is_string(pValue) &&
      (found = states.find(json_string_value(pValue))) != notFound)
    {
      mpExitState = found->second;
    }

  mValid &= (mpExitState != NULL);

  pValue = json_object_get(json, "probability");

  if (json_is_real(pValue))
    {
      mProbability = json_real_value(pValue);
    }

  mValid &= (0.0 <= mProbability && mProbability <= 1.0);

  pValue = json_object_get(json, "dwellTime");

  if (json_is_object(pValue))
    {
      mDwellTime.fromJSON(pValue);
    }

  mValid &= mDwellTime.isValid();

  pValue = json_object_get(json, "susceptibilityFactorOperation");

  if (json_is_object(pValue))
    {
      mSusceptibilityFactorOperation.fromJSON(pValue);
      mValid &= mSusceptibilityFactorOperation.isValid();
    }

  pValue = json_object_get(json, "infectivityFactorOperation");

  if (json_is_object(pValue))
    {
      mInfectivityFactorOperation.fromJSON(pValue);
      mValid &= mInfectivityFactorOperation.isValid();
    }

  Annotation::fromJSON(json);
}

const std::string & Progression::getId() const
{
  return mId;
}

const State * Progression::getEntryState() const
{
  return mpEntryState;
}

const bool & Progression::isValid() const
{
  return mValid;
}
