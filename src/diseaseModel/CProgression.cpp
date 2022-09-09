// BEGIN: Copyright 
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include <jansson.h>

#include "diseaseModel/CProgression.h"
#include "diseaseModel/CHealthState.h"
#include "utilities/CLogger.h"

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
  mValid = false; // DONE

  std::map< std::string, CHealthState * >::const_iterator found;
  std::map< std::string, CHealthState * >::const_iterator notFound = states.end();

  json_t * pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
      mId = json_string_value(pValue);
      mAnnId = mId;
    }

  if (mId.empty())
    {
      CLogger::error("Transition: Invalid or missing 'is'.");
      return;
    }

  CAnnotation::fromJSON(json);

  pValue = json_object_get(json, "entryState");

  if (json_is_string(pValue)
      && (found = states.find(json_string_value(pValue))) != notFound)
    {
      mpEntryState = found->second;
    }

  if (mpEntryState == NULL)
    {
      CLogger::error("Transition: Invalid or missing 'entryState'.");
      return;
    }

  pValue = json_object_get(json, "exitState");

  if (json_is_string(pValue)
      && (found = states.find(json_string_value(pValue))) != notFound)
    {
      mpExitState = found->second;
    }

  if (mpExitState == NULL)
    {
      CLogger::error("Transition: Invalid or missing 'exitState'.");
      return;
    }

  pValue = json_object_get(json, "probability");
  mProbability = -1.0;

  if (json_is_real(pValue))
    {
      mProbability = json_real_value(pValue);
    }

  if (mProbability < 0.0 || 1.0 < mProbability)
    {
      CLogger::error("Transition: Invalid or missing 'probability'.");
      return;
    }

  pValue = json_object_get(json, "dwellTime");

  if (json_is_object(pValue))
    {
      mDwellTime.fromJSON(pValue);
    }

  if (!mDwellTime.isValid())
    {
      CLogger::error("Transition: Invalid or missing 'dwellTime'.");
      return;
    }

  pValue = json_object_get(json, "susceptibilityFactorOperation");

  if (json_is_object(pValue))
    {
      mSusceptibilityFactorOperation.fromJSON(pValue);

      if (!mSusceptibilityFactorOperation.isValid())
        {
          CLogger::error("Transition: Invalid 'susceptibilityFactorOperation'.");
          return;
        }
    }

  pValue = json_object_get(json, "infectivityFactorOperation");

  if (json_is_object(pValue))
    {
      mInfectivityFactorOperation.fromJSON(pValue);
      
      if (!mInfectivityFactorOperation.isValid())
        {
          CLogger::error("Transition: Invalid 'infectivityFactorOperation'.");
          return;
        }
    }

  mValid = true;
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

const double & CProgression::getProbability() const
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
