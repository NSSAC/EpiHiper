// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2023 Rector and Visitors of the University of Virginia 
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
#include "diseaseModel/CProgression.h"
#include "utilities/CLogger.h"
#include "utilities/CMetadata.h"

// static
const CProgression * CHealthState::defaultMethod(const CHealthState * pHealthState, const CNode * /* pNode */)
{
  const PossibleProgressions & Progressions = pHealthState->getPossibleProgressions();

  if (Progressions.A0 > 0.0
      && !Progressions.Progressions.empty())
    {
      double alpha = CRandom::uniform_real(0.0, Progressions.A0)(CRandom::G.Active());

      for (const CProgression * pProgression : Progressions.Progressions)
        {
          alpha -= pProgression->getPropensity();

          if (alpha < 0.0)
            return pProgression;
        }

      return Progressions.Progressions.back();
    }

  return NULL;
}

CHealthState::CHealthState()
  : CAnnotation()
  , CCustomMethod(&CHealthState::defaultMethod)
  , mId()
  , mIndex(std::numeric_limits< size_t >::max())
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
  , CCustomMethod(src)
  , mId(src.mId)
  , mIndex(src.mIndex)
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

const bool & CHealthState::isValid() const
{
  return mValid;
}

const std::string & CHealthState::getId() const
{
  return mId;
}

void CHealthState::setIndex(const size_t & index)
{
  mIndex = index;
}

const size_t & CHealthState::getIndex() const
{
  return mIndex;
}

const double & CHealthState::getSusceptibility() const
{
  return mSusceptibility;
}

const double & CHealthState::getInfectivity() const
{
  return mInfectivity;
}

std::string & CHealthState::getId()
{
  return mId;
}

size_t & CHealthState::getIndex()
{
  return mIndex;
}

double & CHealthState::getSusceptibility()
{
  return mSusceptibility;
}

double & CHealthState::getInfectivity()
{
  return mInfectivity;
}

bool CHealthState::setSusceptibility(const double & value, CValueInterface::pOperator pOperator, const CMetadata & ENABLE_TRACE(metadata))
{
  ENABLE_TRACE(CLogger::trace("CHealthState [ActionDefinition:{}]: Health State ({}) susceptibility {} {}",
                              metadata.contains("CActionDefinition") ? metadata.getInt("CActionDefinition") : -1,
                              mId,
                              CValueInterface::operatorToString(pOperator),
                              value););
  (*pOperator)(mSusceptibility, value);

  return true;
}

bool CHealthState::setInfectivity(const double & value, CValueInterface::pOperator pOperator, const CMetadata & ENABLE_TRACE(metadata))
{
  ENABLE_TRACE(CLogger::trace("CHealthState [ActionDefinition:{}]: Health State () infectivity {} {}",
                              metadata.contains("CActionDefinition") ? metadata.getInt("CActionDefinition") : -1,
                              mId,
                              CValueInterface::operatorToString(pOperator),
                              value););
  (*pOperator)(mInfectivity, value);

  return true;
}


const CHealthState::PossibleProgressions & CHealthState::getPossibleProgressions() const
{
  return mProgressions;
}

void CHealthState::addProgression(const CProgression * pProgression)
{
  if (mProgressions.Progressions.size() == 0)
    {
      mProgressions.A0 = 0.0;
    }

  mProgressions.A0 += pProgression->getPropensity();
  mProgressions.Progressions.push_back(pProgression);
}

void CHealthState::updatePossibleProgression()
{
  mProgressions.A0 = 0.0;

  for (const CProgression * pProgression : mProgressions.Progressions)
    mProgressions.A0 += pProgression->getPropensity();
}

const CProgression * CHealthState::nextProgression(const CNode * pNode) const
{
  return mpCustomMethod(this, pNode);
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
