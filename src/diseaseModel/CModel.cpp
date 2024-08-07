// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2024 Rector and Visitors of the University of Virginia 
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

#include <fstream>
#include <jansson.h>

#include "diseaseModel/CModel.h"

#include "utilities/CSimConfig.h"
#include "diseaseModel/CHealthState.h"
#include "diseaseModel/CProgression.h"
#include "diseaseModel/CTransmission.h"
#include "actions/CActionQueue.h"
#include "actions/CProgressionAction.h"
#include "actions/CTransmissionAction.h"
#include "network/CEdge.h"
#include "network/CNetwork.h"
#include "network/CNode.h"
#include "utilities/CRandom.h"
#include "utilities/CLogger.h"
#include "variables/CVariableList.h"

// static
void CModel::Load(const std::string & modelFile)
{
  if (INSTANCE == NULL)
    {
      INSTANCE = new CModel(modelFile);
    }
}

// static
void CModel::clear()
{
  if (INSTANCE != NULL)
    {
      delete INSTANCE;
      INSTANCE = NULL;
    }
}

CModel::CModel(const std::string & modelFile)
  : CAnnotation()
  , mStates()
  , mStateCount(0)
  , mId2State()
  , mpInitialState(NULL)
  , mTransmissions()
  , mProgressions()
  , mPossibleTransmissions(NULL)
  , mpTransmissibility(NULL)
  , mValid(false)
{
  CVariableList::INSTANCE.append(CVariable::transmissibility());
  mpTransmissibility = &CVariableList::INSTANCE["%transmissibility%"];

  json_t * pRoot = CSimConfig::loadJson< CLogger::error >(modelFile, JSON_DECODE_INT_AS_REAL);

  if (pRoot != NULL)
    {
      fromJSON(pRoot);
    }

  json_decref(pRoot);
}

// virtual
CModel::~CModel()
{}

void CModel::fromJSON(const json_t * json)
{
  /*
  {
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "diseaseModel",
  "title": "Disease Model",
  "description": "The disease model describes the possible disease state changes within an individual including transmission.",
  "type": "object",
  "required": [
    "epiHiperSchema",
    "states",
    "initialState",
    "transmissions",
    "transitions"
  ],
  "properties": {
    "epiHiperSchema": {
      "type": "string",
      "enum": ["https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/diseaseModelSchema.json"]
    },
    "states": {
      "type": "array",
      "description": "An array of states",
      "items": {}
    },
    "initialState": {"$ref": "./typeRegistry.json#/definitions/uniqueIdRef"},
    "transmissions": {
      "type": "array",
      "description": "An array listing possible transmissions due to contact with other nodes.",
      "items": {}
    },
    "transitions": {
      "type": "array",
      "description": "The transitions specifying the disease progression.",
      "items": {}
    },
    "transmissability": {
      "description": "The overal transmissability",
      "default": 1.0,
      "$ref": "./typeRegistry.json#/definitions/nonNegativeNumber"
    }
  }
}
*/
  mValid = false; // DONE

  json_t * pValue = json_object_get(json, "states");

  mStateCount = json_array_size(pValue);
  mStates.resize(mStateCount);
  mPossibleTransmissions = new PossibleTransmissions[mStateCount];
  
  PossibleTransmissions * pTransmissions = mPossibleTransmissions;
  std::vector< CHealthState >::iterator pState = mStates.begin();

  for (size_t i = 0; i < mStateCount; ++i, ++pState, ++pTransmissions)
    {
      pState->fromJSON(json_array_get(pValue, i));
      pTransmissions->Transmissions = NULL;

      if (pState->isValid())
        {
          pState->setIndex(i);
          mId2State[pState->getId()] = &*pState;
        }
      else
        {
          CLogger::error("Model: Invalid state for item '{}'.", i);
          return;
        }
    }

  pValue = json_object_get(json, "initialState");
  mpInitialState = NULL;

  if (json_is_string(pValue))
    {
      std::map< std::string, CHealthState * >::const_iterator found = mId2State.find(json_string_value(pValue));

      if (found != mId2State.end())
        {
          mpInitialState = found->second;
        }
    }

  if (mpInitialState == NULL)
    {
      CLogger::error("Model: Invalid or missing 'initialState'.");
      return;
    }

  pValue = json_object_get(json, "transmissions");

  mTransmissions.resize(json_array_size(pValue));
  std::vector< CTransmission >::iterator itTrans = mTransmissions.begin();
  std::vector< CTransmission >::iterator endTrans = mTransmissions.end();

  for (size_t i = 0; itTrans != endTrans; ++itTrans, ++i)
    {
      itTrans->fromJSON(json_array_get(pValue, i), mId2State);

      if (itTrans->isValid())
        {
          state_t StateType =itTrans->getEntryState()->getIndex();

          if (mPossibleTransmissions[StateType].Transmissions == NULL)
            {
              mPossibleTransmissions[StateType].Transmissions = new CTransmission*[mStateCount];
              memset(mPossibleTransmissions[StateType].Transmissions, 0, sizeof(void *) * mStateCount);
            }

          mPossibleTransmissions[StateType].Transmissions[itTrans->getContactState()->getIndex()] = &*itTrans;
        }
      else
        {
          CLogger::error("Model: Invalid transmission for item '{}'.", i);
          return;
        }
    }

  pValue = json_object_get(json, "transitions");

  mProgressions.resize(json_array_size(pValue));
  std::vector< CProgression >::iterator itProg = mProgressions.begin();
  std::vector< CProgression >::iterator endProg = mProgressions.end();

  for (size_t i = 0; itProg != endProg; ++itProg, ++i)
    {
      itProg->fromJSON(json_array_get(pValue, i), mId2State);

      if (itProg->isValid())
        {
          const_cast< CHealthState * >(itProg->getEntryState())->addProgression(&*itProg);
        }
      else
        {
          CLogger::error("Model: Invalid transition for item '{}'.", i);
          return;
        }
    }

  pValue = json_object_get(json, "transmissibility");

  if (json_is_real(pValue))
    {
      CVariableList::INSTANCE["%transmissibility%"].setInitialValue(json_real_value(pValue));
    }

  mValid = true;
  CAnnotation::fromJSON(json);
}

// static
const CHealthState & CModel::GetInitialState()
{
  return *INSTANCE->mpInitialState;
}

// static
CHealthState * CModel::GetState(const std::string & id)
{
  std::map< std::string, CHealthState * >::const_iterator found = INSTANCE->mId2State.find(id);

  if (found != INSTANCE->mId2State.end())
    return found->second;

  return NULL;
}

// static
CHealthState * CModel::StateFromType(const CModel::state_t & type)
{
  return INSTANCE->stateFromType(type);
}

// static
const bool & CModel::isValid()
{
  return INSTANCE->mValid;
}

// static
bool CModel::ProcessTransmissions()
{
  return INSTANCE->processTransmissions();
}

bool CModel::processTransmissions() const
{
  bool success = true;
  double resolutionPerTick = 1.0 / CNetwork::timeResolution();

  CRandom::uniform_real Uniform01(0.0, 1.0);

  CNode * pNode = CNetwork::Context.Active().beginNode();
  CNode * pNodeEnd = CNetwork::Context.Active().endNode();

  struct Candidate
  {
    const CEdge * pEdge;
    const CTransmission * pTransmission;
    double Propensity;

    Candidate(const CEdge * edge, const CTransmission * transmission, double propensity)
      : pEdge(edge)
      , pTransmission(transmission)
      , Propensity(propensity)
    {}
  };

  std::vector< Candidate > Candidates;
  CTransmission ** pPossibleTransmissions = NULL;
  double Transmissibility = mpTransmissibility->toValue().toNumber();

  for (pNode = CNetwork::Context.Active().beginNode(); pNode != pNodeEnd; ++pNode)
    if (pNode->susceptibility > 0.0
        && (pPossibleTransmissions = mPossibleTransmissions[pNode->healthState].Transmissions) != NULL)
      {
        CEdge * pEdge = pNode->Edges;
        CEdge * pEdgeEnd = pNode->Edges + pNode->EdgesSize;

        CTransmission * pTransmission = NULL;

        Candidates.clear();
        double A0 = 0.0;

        for (; pEdge != pEdgeEnd; ++pEdge)
          {
            if (pEdge->active
                && pEdge->pSource->infectivity > 0.0
                && (pTransmission = pPossibleTransmissions[pEdge->pSource->healthState]) != NULL)
              {
                double Propensity = pTransmission->propensity(pEdge);

                if (Propensity > 0.0)
                  {
                    A0 += Propensity;
                    Candidates.emplace_back(pEdge, pTransmission, Propensity);
                  }
              }
          }

        if (A0 > 0.0
            && -log(Uniform01(CRandom::G.Active())) < A0 * Transmissibility * resolutionPerTick)
          {
            double alpha = Uniform01(CRandom::G.Active()) * A0;

            std::vector< Candidate >::const_iterator itCandidate = Candidates.begin();
            std::vector< Candidate >::const_iterator endCandidate = Candidates.end();

            for (; itCandidate != endCandidate; ++itCandidate)
              {
                alpha -= itCandidate->Propensity;

                if (alpha < 0.0)
                  break;
              }

            try
              {
                const Candidate & Candidate = (itCandidate != endCandidate) ? *itCandidate : *Candidates.rbegin();
                CActionQueue::addAction(0, new CTransmissionAction(Candidate.pTransmission, pNode, Candidate.pEdge));
              }
            catch (...)
              {
                CLogger::error("CModel:: Failed to create transmission for '{}'.", pNode->id);
              }
          }
      }

  return success;
}

// static
void CModel::StateChanged(CNode * pNode)
{
  INSTANCE->stateChanged(pNode);
}

void CModel::stateChanged(CNode * pNode) const
{
  // std::cout << pNode->id << ": " << pNode->Edges << ", " << pNode->Edges + pNode->EdgesSize << ", " << pNode->EdgesSize << std::endl;
  const CProgression * pProgression = pNode->getHealthState()->nextProgression(pNode);

  if (pProgression != NULL)
    {
      try
        {
          CActionQueue::addAction(pProgression->dwellTime(pNode), new CProgressionAction(pProgression, pNode));
        }
      catch (...)
        {
          CLogger::error("CModel:: Failed to create progression for '{}'.", pNode->id);
        }
    }
}

const std::vector< CHealthState > & CModel::getStates() const
{
  return mStates;
}

const size_t & CModel::getStateCount() const
{
  return mStateCount;
}

const std::vector< CTransmission > &  CModel::getTransmissions() const
{
  return mTransmissions;
}

const std::vector< CProgression > & CModel::getProgressions() const
{
  return mProgressions;
}

CHealthState * CModel::stateFromType(const CModel::state_t & type) {
  return &mStates[type];
}

// static
const std::vector< CTransmission > & CModel::GetTransmissions()
{
  return INSTANCE->getTransmissions();
}

// static 
CTransmission * CModel::GetTransmission(const std::string & id)
{
  CTransmission * pTransmission = nullptr;

  for (const CTransmission & Transmission : CModel::GetTransmissions())
    if (Transmission.getId() == id)
      {
        pTransmission = const_cast< CTransmission * >(&Transmission);
        break;
      }

  return pTransmission;
}

// static 
const std::vector< CHealthState > & CModel::GetStates()
{
  return CModel::INSTANCE->getStates();
}

// static 
const std::vector< CProgression > & CModel::GetProgressions()
{
  return CModel::INSTANCE->getProgressions();
}

// static 
CProgression * CModel::GetProgression(const std::string & id)
{
  CProgression * pProgression = nullptr;

  for (const CProgression & Progression : CModel::GetProgressions())
    if (Progression.getId() == id)
      {
        pProgression = const_cast< CProgression * >(&Progression);
        break;
      }

  return pProgression;
}

// static
int CModel::UpdateGlobalStateCounts()
{
  size_t Size = INSTANCE->mStateCount;
  CHealthState::Counts LocalStateCounts[Size];

  std::vector< CHealthState >::iterator pState = INSTANCE->mStates.begin();
  std::vector< CHealthState >::iterator pStateEnd = INSTANCE->mStates.end();
  CHealthState::Counts * pLocalCount = LocalStateCounts;

  for (; pState != pStateEnd; ++pState, ++pLocalCount)
    {
      CContext< CHealthState::Counts > & StateCounts = pState->getLocalCounts();
      pLocalCount->Current = 0;
      pLocalCount->In = 0;
      pLocalCount->Out = 0;
      CHealthState::Counts * pIt = StateCounts.beginThread();
      CHealthState::Counts * pEnd = StateCounts.endThread();

      for (; pIt != pEnd; ++pIt)
        {
          pLocalCount->Current += pIt->Current;
          pLocalCount->In += pIt->In;
          pIt->In = 0;
          pLocalCount->Out += pIt->Out;
          pIt->Out = 0;
        }

      pState->setGlobalCounts(*pLocalCount);
    }

  CCommunicate::Receive ReceiveCounts(&CModel::ReceiveGlobalStateCounts);
  CCommunicate::roundRobinFixed(LocalStateCounts, Size * sizeof(CHealthState::Counts), &ReceiveCounts);

  return (int) CCommunicate::ErrorCode::Success;
}

// static
CCommunicate::ErrorCode CModel::ReceiveGlobalStateCounts(std::istream & is, int /* sender */)
{
  CHealthState::Counts Increment;
  std::vector< CHealthState >::iterator pState = INSTANCE->mStates.begin();
  std::vector< CHealthState >::iterator pStateEnd = INSTANCE->mStates.end();

  for (; pState != pStateEnd; ++pState)
    {
      is.read(reinterpret_cast< char * >(&Increment), sizeof(CHealthState::Counts));
      pState->incrementGlobalCount(Increment);
    }

  return CCommunicate::ErrorCode::Success;
}

// static
void CModel::InitGlobalStateCountOutput()
{
  if (CCommunicate::MPIRank == 0)
    {
      std::ofstream out;

      out.open(CSimConfig::getSummaryOutput().c_str());

      if (out.good())
        {
          std::vector< CHealthState >::iterator pState = INSTANCE->mStates.begin();
          std::vector< CHealthState >::iterator pStateEnd = INSTANCE->mStates.end();
          bool first = true;

          // Loop through all states
          for (; pState != pStateEnd; ++pState)
            {
              if (first)
                {
                  out << "tick";
                  first = false;
                }

              out << "," << pState->getId() << "[current]," << pState->getId() << "[in]," << pState->getId() << "[out]";
            }

          // We also add variables
          CVariableList::const_iterator it = CVariableList::INSTANCE.begin();
          CVariableList::const_iterator end = CVariableList::INSTANCE.end();

          for (; it != end; ++it)
            out << "," << (*it)->getId() << (((*it)->getScope() == CVariable::Scope::global) ? "(g)" : "(l)");

          out << ",seed" << std::endl;
        }
      else
        {
          CLogger::error("Error (Rank 0): Failed to open file '" + CSimConfig::getSummaryOutput() + "'.");
          exit(EXIT_FAILURE);
        }

      out.close();
    }
}

// static
bool CModel::WriteGlobalStateCounts()
{
  if (CCommunicate::MPIRank == 0)
    {
      std::ofstream out;

      out.open(CSimConfig::getSummaryOutput().c_str(), std::ios_base::app);

      if (out.fail())
        {
          CLogger::error("CModel::WriteGlobalStateCounts: Failed to open '{}'.", CSimConfig::getSummaryOutput());
          return false;
        }
      else
        {
          std::vector< CHealthState >::iterator pState = INSTANCE->mStates.begin();
          std::vector< CHealthState >::iterator pStateEnd = INSTANCE->mStates.end();

          out << CActionQueue::getCurrentTick();

          // Loop through all states
          for (; pState != pStateEnd && out.good(); ++pState)
              {
                const CHealthState::Counts & Counts = pState->getGlobalCounts();

                out << "," << Counts.Current << "," << Counts.In << "," << Counts.Out;
              }

          // We also add variables
          CVariableList::const_iterator it = CVariableList::INSTANCE.begin();
          CVariableList::const_iterator end = CVariableList::INSTANCE.end();

          for (; it != end && out.good(); ++it)
              {
                out << "," << (*it)->toValue().toNumber();
              }

          out << "," << CRandom::getSeed() << std::endl;
        }

      if (out.fail())
        {
          CLogger::error("CModel::WriteGlobalStateCounts: Failed to write '{}'.", CSimConfig::getSummaryOutput());
          return false;
        }

      out.close();

      if (out.fail())
        {
          CLogger::error("CModel::WriteGlobalStateCounts: Failed to close '{}'.", CSimConfig::getSummaryOutput());
          return false;
        }
    }

    return true;
}
