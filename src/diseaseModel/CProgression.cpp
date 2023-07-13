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

#include "diseaseModel/CProgression.h"
#include "diseaseModel/CHealthState.h"
#include "utilities/CLogger.h"

// static
unsigned int CProgression::defaultMethod(const CProgression * pProgression, const CNode * /* pNode */)
{
  return pProgression->mDwellTime.sample();
}

CProgression::CProgression()
  : CAnnotation()
  , CCustomMethod(&CProgression::defaultMethod)
  , mId()
  , mpEntryState(NULL)
  , mpExitState(NULL)
  , mPropensity(0)
  , mDwellTime()
  , mSusceptibilityFactorOperation()
  , mInfectivityFactorOperation()
  , mValid(false)
{}

CProgression::CProgression(const CProgression & src)
  : CAnnotation(src)
  , CCustomMethod(src)
  , mId(src.mId)
  , mpEntryState(src.mpEntryState)
  , mpExitState(src.mpExitState)
  , mPropensity(src.mPropensity)
  , mDwellTime(src.mDwellTime)
  , mSusceptibilityFactorOperation(src.mSusceptibilityFactorOperation)
  , mInfectivityFactorOperation(src.mInfectivityFactorOperation)
  , mValid(false)
{}

// virtual
CProgression::~CProgression()
{}

// virtual 
void CProgression::fromJSON(const json_t * json)
{
  CAnnotation::fromJSON(json);
}


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

  fromJSON(json);

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

  mPropensity = -1.0;
  pValue = json_object_get(json, "probability");

  if (json_is_real(pValue))
    {
      mPropensity = json_real_value(pValue);

      if (mPropensity < 0.0 || 1.0 < mPropensity)
        {
          CLogger::error("Transition: Invalid 'probability'.");
          return;
        }
    }

  pValue = json_object_get(json, "propensity");

  if (json_is_real(pValue))
    {
      mPropensity = json_real_value(pValue);

      if (mPropensity < 0.0)
        {
          CLogger::error("Transition: Invalid 'propensity'.");
          return;
        }
    }

  if (mPropensity < 0.0)
    {
      CLogger::error("Transition: Missing 'propensity' or 'probability'.");
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

const double & CProgression::getPropensity() const
{
  return mPropensity;
}

std::string & CProgression::getId()
{
  return mId;
}

CHealthState * CProgression::getEntryState()
{
  return const_cast< CHealthState * >(mpEntryState);
}

CHealthState * CProgression::getExitState()
{
  return const_cast< CHealthState * >(mpExitState);
}

double & CProgression::getPropensity()
{
  return mPropensity;
}

std::string & CProgression::getDwellTime()
{
  return mDwellTime.getJson();
}

std::string & CProgression::getSusceptibilityFactorOperation()
{
  return mSusceptibilityFactorOperation.getJson();
}

std::string & CProgression::getInfectivityFactorOperation()
{
  return mInfectivityFactorOperation.getJson();
}

bool CProgression::setPropensity(const double & value, CValueInterface::pOperator pOperator, const CMetadata & ENABLE_TRACE(metadata))
{
  ENABLE_TRACE(CLogger::trace("CProgression [ActionDefinition:{}]: progression ({}) propensity {} {}",
                              metadata.contains("CActionDefinition") ? metadata.getInt("CActionDefinition") : -1,
                              mId,
                              CValueInterface::operatorToString(pOperator),
                              value););
  (*pOperator)(mPropensity, value);

  const_cast< CHealthState * >(mpEntryState)->updatePossibleProgression();

  return true;
}

bool CProgression::setDwellTime(const std::string & value, CValueInterface::pOperator ENABLE_TRACE(pOperator), const CMetadata & ENABLE_TRACE(metadata))
{
  ENABLE_TRACE(CLogger::trace("CProgression [ActionDefinition:{}]: progression ({}) dwell time {} {}",
                              metadata.contains("CActionDefinition") ? metadata.getInt("CActionDefinition") : -1,
                              mId,
                              CValueInterface::operatorToString(pOperator),
                              value););

  return mDwellTime.setJson(value);
}

bool CProgression::setSusceptibilityFactorOperation(const std::string & value, CValueInterface::pOperator ENABLE_TRACE(pOperator), const CMetadata & ENABLE_TRACE(metadata))
{
  ENABLE_TRACE(CLogger::trace("CProgression [ActionDefinition:{}]: progression ({}) susceptibility factor operation {} {}",
                              metadata.contains("CActionDefinition") ? metadata.getInt("CActionDefinition") : -1,
                              mId,
                              CValueInterface::operatorToString(pOperator),
                              value););

  return mSusceptibilityFactorOperation.setJson(value);
}

bool CProgression::setInfectivityFactorOperation(const std::string & value, CValueInterface::pOperator ENABLE_TRACE(pOperator), const CMetadata & ENABLE_TRACE(metadata))
{
  ENABLE_TRACE(CLogger::trace("CProgression [ActionDefinition:{}]: progression ({}) infectivity factor operation {} {}",
                              metadata.contains("CActionDefinition") ? metadata.getInt("CActionDefinition") : -1,
                              mId,
                              CValueInterface::operatorToString(pOperator),
                              value););

  return mInfectivityFactorOperation.setJson(value);
}
unsigned int CProgression::dwellTime(const CNode * pNode) const
{
  return mpCustomMethod(this, pNode);
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
