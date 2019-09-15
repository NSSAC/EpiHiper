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

#include "diseaseModel/CHealthState.h"

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
  mValid = true;

  json_t * pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
      mId = json_string_value(pValue);
      mAnnId = mId;
    }

  mValid &= !mId.empty();

  pValue = json_object_get(json, "susceptibility");

  if (json_is_real(pValue))
    {
      mSusceptibility = json_real_value(pValue);
    }

  pValue = json_object_get(json, "infectivity");

  if (json_is_real(pValue))
    {
      mInfectivity = json_real_value(pValue);
    }

  CAnnotation::fromJSON(json);
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

