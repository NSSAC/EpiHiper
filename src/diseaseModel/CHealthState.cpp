// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
//  
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal 
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions: 
//  
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software. 
//  
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE 
// END: Copyright 

#include <jansson.h>

#include "diseaseModel/CHealthState.h"
#include "utilities/CLogger.h"

CHealthState::CHealthState()
  : CAnnotation()
  , mId()
  , mSusceptibility(-1.0)
  , mInfectivity(-1.0)
  , mValid(false)
  , mLocalCounts()
  , mGlobalCounts()
{
  mLocalCounts.init();

  Counts & Master = mLocalCounts.Master();
  Master.Current = 0;
  Master.Out = 0;
  Master.In = 0;

  Counts * pIt = mLocalCounts.beginThread();
  Counts * pEnd = mLocalCounts.endThread();

  for (; pIt != pEnd; ++pIt)
    if (mLocalCounts.isThread(pIt))
      {
        pIt->Current = 0;
        pIt->Out = 0;
        pIt->In = 0;
      }
}

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
{
  mLocalCounts.release();
}

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

const CContext< CHealthState::Counts > & CHealthState::getLocalCounts() const
{
  return mLocalCounts;
}

CContext< CHealthState::Counts > & CHealthState::getLocalCounts()
{
  return mLocalCounts;
}

const CHealthState::Counts & CHealthState::getGlobalCounts() const
{
  return mGlobalCounts;
}

void CHealthState::setGlobalCounts(const Counts & counts)
{
  mGlobalCounts = counts;
}

void CHealthState::incrementGlobalCount(const CHealthState::Counts & i)
{
  mGlobalCounts.Current += i.Current;
  mGlobalCounts.In += i.In;
  mGlobalCounts.Out += i.Out;
}
