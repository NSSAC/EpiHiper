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

#include "CProgression.h"

#include <jansson.h>

#include "CHealthState.h"

CProgression::CProgression()
  : CAnnotation()
  , mId()
  , mpEntryState(NULL)
  , mpExitState(NULL)
  , mProbability(0)
  , mDwellTime()
  , mSusceptibilityFactorOperation()
  , mInfectivityFactorOperation()
  , mValid(false)
{}

CProgression::CProgression(const CProgression & src)
  : CAnnotation(src)
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
CProgression::~CProgression()
{}

void CProgression::fromJSON(const json_t * json, const std::map< std::string, CHealthState * > & states)
{
  mValid = true;

  std::map< std::string, CHealthState * >::const_iterator found;
  std::map< std::string, CHealthState * >::const_iterator notFound = states.end();

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

  CAnnotation::fromJSON(json);
}

const std::string & CProgression::getId() const
{
  return mId;
}

const CHealthState * CProgression::getEntryState() const
{
  return mpEntryState;
}

const CHealthState * CProgression::getExitState() const
{
  return mpExitState;
}

const double & CProgression::getPropability() const
{
  return mProbability;
}

unsigned int CProgression::getDwellTime() const
{
  return mDwellTime.sample();
}

void CProgression::updateSusceptibilityFactor(double & factor) const
{
  mSusceptibilityFactorOperation.apply(factor);
}

void CProgression::updateInfectivityFactor(double & factor) const
{
  mInfectivityFactorOperation.apply(factor);
}

const bool & CProgression::isValid() const
{
  return mValid;
}
