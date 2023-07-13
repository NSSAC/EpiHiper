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

#include "diseaseModel/CTransmission.h"
#include "diseaseModel/CHealthState.h"
#include "network/CEdge.h"
#include "network/CNode.h"
#include "utilities/CLogger.h"

// static
double CTransmission::defaultMethod(const CTransmission * pTransmission, const CEdge * pEdge)
{
  // ρ(P, P', Τi,j,k) = (| contactTime(P, P') ∩ [tn, tn + Δtn] |) × contactWeight(P, P') × σ(P, Χi) × ι(P',Χk) × ω(Τi,j,k)
  return pEdge->duration * pEdge->weight * pEdge->pTarget->susceptibility
         * pEdge->pSource->infectivity * pTransmission->getTransmissibility();
}

CTransmission::CTransmission()
  : CAnnotation()
  , CCustomMethod(&CTransmission::defaultMethod)
  , mId()
  , mpEntryState(NULL)
  , mpExitState(NULL)
  , mpContactState(NULL)
  , mTransmissibility(0)
  , mSusceptibilityFactorOperation()
  , mInfectivityFactorOperation()
  , mValid(false)
{}

CTransmission::CTransmission(const CTransmission & src)
  : CAnnotation(src)
  , CCustomMethod(src)
  , mId(src.mId)
  , mpEntryState(src.mpEntryState)
  , mpExitState(src.mpExitState)
  , mpContactState(src.mpContactState)
  , mTransmissibility(src.mTransmissibility)
  , mSusceptibilityFactorOperation(src.mSusceptibilityFactorOperation)
  , mInfectivityFactorOperation(src.mInfectivityFactorOperation)
  , mValid(false)
{}

// virtual
CTransmission::~CTransmission()
{}

// virtual 
void CTransmission::fromJSON(const json_t * json)
{
  CAnnotation::fromJSON(json);
}

void CTransmission::fromJSON(const json_t * json, const std::map< std::string, CHealthState * > & states)
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

  if (mId.empty())
    {
      CLogger::error("Transmission: Invalid or missing 'id'.");
      return;
    }

  fromJSON(json);

  pValue = json_object_get(json, "entryState");
  mpEntryState = NULL;

  if (json_is_string(pValue)
      && (found = states.find(json_string_value(pValue))) != notFound)
    {
      mpEntryState = found->second;
    }

  if (mpEntryState == NULL)
    {
      CLogger::error("Transmission: Invalid or missing 'entryState'.");
      return;
    }

  pValue = json_object_get(json, "exitState");
  mpExitState = NULL;

  if (json_is_string(pValue)
      && (found = states.find(json_string_value(pValue))) != notFound)
    {
      mpExitState = found->second;
    }

  if (mpExitState == NULL)
    {
      CLogger::error("Transmission: Invalid or missing 'exitState'.");
      return;
    }

  pValue = json_object_get(json, "contactState");
  mpContactState = NULL;

  if (json_is_string(pValue)
      && (found = states.find(json_string_value(pValue))) != notFound)
    {
      mpContactState = found->second;
    }

  if (mpContactState == NULL)
    {
      CLogger::error("Transmission: Invalid or missing 'contactState'.");
      return;
    }

  pValue = json_object_get(json, "transmissibility");
  mTransmissibility = -1.0;

  if (json_is_real(pValue))
    {
      mTransmissibility = json_real_value(pValue);
    }

  if (mTransmissibility < 0.0)
    {
      CLogger::error("Transmission: Invalid or missing 'transmissibility'.");
      return;
    }

  pValue = json_object_get(json, "susceptibilityFactorOperation");

  if (json_is_object(pValue))
    {
      mSusceptibilityFactorOperation.fromJSON(pValue);

      if (!mSusceptibilityFactorOperation.isValid())
        {
          CLogger::error("Transmission: Invalid 'susceptibilityFactorOperation'.");
          return;
        }
    }

  pValue = json_object_get(json, "infectivityFactorOperation");

  if (json_is_object(pValue))
    {
      mInfectivityFactorOperation.fromJSON(pValue);

      if (!mInfectivityFactorOperation.isValid())
        {
          CLogger::error("Transmission: Invalid 'infectivityFactorOperation'.");
          return;
        }
    }

  mValid = true;
}

const std::string & CTransmission::getId() const
{
  return mId;
}

const CHealthState * CTransmission::getEntryState() const
{
  return mpEntryState;
}

const CHealthState * CTransmission::getContactState() const
{
  return mpContactState;
}

const CHealthState * CTransmission::getExitState() const
{
  return mpExitState;
}

const double & CTransmission::getTransmissibility() const
{
  return mTransmissibility;
}

std::string & CTransmission::getId()
{
  return mId;
}

CHealthState * CTransmission::getEntryState()
{
  return const_cast< CHealthState * >(mpEntryState);
}

CHealthState * CTransmission::getContactState()
{
  return const_cast< CHealthState * >(mpContactState);
}

CHealthState * CTransmission::getExitState()
{
  return const_cast< CHealthState * >(mpExitState);
}

double & CTransmission::getTransmissibility()
{
  return mTransmissibility;
}

std::string & CTransmission::getSusceptibilityFactorOperation()
{
  return mSusceptibilityFactorOperation.getJson();
}

std::string & CTransmission::getInfectivityFactorOperation()
{
  return mInfectivityFactorOperation.getJson();
}

const bool & CTransmission::isValid() const
{
  return mValid;
}

bool CTransmission::setTransmissibility(const double & value, CValueInterface::pOperator pOperator, const CMetadata & ENABLE_TRACE(metadata))
{
  ENABLE_TRACE(CLogger::trace("CTransmission [ActionDefinition:{}]: transmission ({}) transmissibility {} {}",
                              metadata.contains("CActionDefinition") ? metadata.getInt("CActionDefinition") : -1,
                              mId,
                              CValueInterface::operatorToString(pOperator),
                              value););
  (*pOperator)(mTransmissibility, value);

  return true;
}

bool CTransmission::setSusceptibilityFactorOperation(const std::string & value, CValueInterface::pOperator ENABLE_TRACE(pOperator), const CMetadata & ENABLE_TRACE(metadata))
{
  ENABLE_TRACE(CLogger::trace("CTransmission [ActionDefinition:{}]: transmission ({}) susceptibility factor operation {} {}",
                              metadata.contains("CActionDefinition") ? metadata.getInt("CActionDefinition") : -1,
                              mId,
                              CValueInterface::operatorToString(pOperator),
                              value););

  return mSusceptibilityFactorOperation.setJson(value);
}

bool CTransmission::setInfectivityFactorOperation(const std::string & value, CValueInterface::pOperator ENABLE_TRACE(pOperator), const CMetadata & ENABLE_TRACE(metadata))
{
  ENABLE_TRACE(CLogger::trace("CTransmission [ActionDefinition:{}]: transmission ({}) infectivity factor operation {} {}",
                              metadata.contains("CActionDefinition") ? metadata.getInt("CActionDefinition") : -1,
                              mId,
                              CValueInterface::operatorToString(pOperator),
                              value););

  return mInfectivityFactorOperation.setJson(value);
}

void CTransmission::updateSusceptibilityFactor(double & factor) const
{
  mSusceptibilityFactorOperation.apply(factor);
}

void CTransmission::updateInfectivityFactor(double & factor) const
{
  mInfectivityFactorOperation.apply(factor);
}

double CTransmission::propensity(const CEdge * pEdge) const
{
  return mpCustomMethod(this, pEdge);
}
