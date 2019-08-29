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

CHealthState::CHealthState()
  : CAnnotation()
  , mId()
  , mSusceptibility(-1.0)
  , mInfectivity(-1.0)
  , mValid(false)
  , mLocalCount(0)
  , mGlobalCount(0)
{}

CHealthState::CHealthState(const CHealthState & src)
  : CAnnotation(src)
  , mId(src.mId)
  , mSusceptibility(src.mSusceptibility)
  , mInfectivity(src.mInfectivity)
  , mValid(src.mValid)
  , mLocalCount(src.mLocalCount)
  , mGlobalCount(src.mGlobalCount)
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

const size_t & CHealthState::getLocalCount() const
{
  return mLocalCount;
}

const size_t & CHealthState::getGlobalCount() const
{
  return mGlobalCount;
}

void CHealthState::resetGlobalCount()
{
  mGlobalCount = mLocalCount;
}

void CHealthState::incrementGlobalCount(const size_t & i)
{
  mGlobalCount += i;
}

