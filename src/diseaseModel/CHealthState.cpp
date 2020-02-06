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

#include <jansson.h>

#include "diseaseModel/CHealthState.h"
#include "utilities/CLogger.h"

CHealthState::Counts::Counts()
  : Current(0)
  , In(0)
  , Out(0)
{}

CHealthState::Counts::Counts(const Counts & src)
  : Current(src.Current)
  , In(src.In)
  , Out(src.Out)
{}

CHealthState::Counts::~Counts()
{}

CHealthState::CHealthState()
  : CAnnotation()
  , mId()
  , mSusceptibility(-1.0)
  , mInfectivity(-1.0)
  , mValid(false)
  , mLocalCounts()
  , mGlobalCounts()
{}

CHealthState::CHealthState(const CHealthState & src)
  : CAnnotation(src)
  , mId(src.mId)
  , mSusceptibility(src.mSusceptibility)
  , mInfectivity(src.mInfectivity)
  , mValid(src.mValid)
  , mLocalCounts(src.mLocalCounts)
  , mGlobalCounts(src.mGlobalCounts)
{}

// virtual
CHealthState::~CHealthState()
{}

void CHealthState::fromJSON(const json_t * json)
{
  mValid = false;

  json_t * pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
      mId = json_string_value(pValue);
      mAnnId = mId;
    }

  if (mId.empty())
    {
      CLogger::error("Health state: Invalid or missing 'id',");
      return;
    }

  CAnnotation::fromJSON(json);

  pValue = json_object_get(json, "susceptibility");
  mSusceptibility = -1.0;

  if (json_is_real(pValue))
    {
      mSusceptibility = json_real_value(pValue);
    }

  if (mSusceptibility < 0.0)
    {
      CLogger::error("Health state: Invalid or missing 'susceptibility'.");
      return;
    }

  pValue = json_object_get(json, "infectivity");
  mInfectivity = -1.0;

  if (json_is_real(pValue))
    {
      mInfectivity = json_real_value(pValue);
    }

  if (mInfectivity < 0.0)
    {
      CLogger::error("Health state: Invalid or missing 'infectivity'.");
      return;
    }

  mValid = true;
}

const std::string & CHealthState::getId() const
{
  return mId;
}

const double & CHealthState::getSusceptibility() const
{
  return mSusceptibility;
}

const double & CHealthState::getInfectivity() const
{
  return mInfectivity;
}

const bool & CHealthState::isValid() const
{
  return mValid;
}

const CHealthState::Counts & CHealthState::getLocalCounts() const
{
  return mLocalCounts;
}

const CHealthState::Counts & CHealthState::getGlobalCounts() const
{
  return mGlobalCounts;
}

void CHealthState::resetCounts()
{
  mGlobalCounts = mLocalCounts;

  mLocalCounts.In = 0;
  mLocalCounts.Out = 0;
}

void CHealthState::incrementGlobalCount(const CHealthState::Counts & i)
{
  mGlobalCounts.Current += i.Current;
  mGlobalCounts.In += i.In;
  mGlobalCounts.Out += i.Out;
}
