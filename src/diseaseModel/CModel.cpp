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

#include <fstream>
#include <jansson.h>

#include "diseaseModel/CModel.h"

#include "utilities/CSimConfig.h"
#include "diseaseModel/CHealthState.h"
#include "diseaseModel/CProgression.h"
#include "diseaseModel/CTransmission.h"
#include "diseaseModel/CTransmissionPropensity.h"
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
void CModel::Release()
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
  , mPossibleProgressions(NULL)
  , mpTransmissibility(NULL)
  , mValid(false)
{
  CVariableList::INSTANCE.append(CVariable::transmissibility());
  mpTransmissibility = &CVariableList::INSTANCE["%transmissibility%"].toNumber();

  json_t * pRoot = CSimConfig::loadJson(modelFile, JSON_DECODE_INT_AS_REAL);

  if (pRoot != NULL)
    {
      fromJSON(pRoot);
    }

  json_decref(pRoot);
}

// virtual
CModel::~CModel()
{
  if (mStates != NULL)
    {
      delete[] mStates;
    }
}

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
  mStates = new CHealthState[mStateCount];
  mPossibleTransmissions = new PossibleTransmissions[mStateCount];
  mPossibleProgressions = new PossibleProgressions[mStateCount];

  PossibleTransmissions * pTransmissions = mPossibleTransmissions;
  PossibleProgressions * pProgressions = mPossibleProgressions;
  CHealthState * pState = mStates;

  for (size_t i = 0; i < mStateCount; ++i, ++pState, ++pTransmissions, ++pProgressions)
    {
      pState->fromJSON(json_array_get(pValue, i));
      pTransmissions->Transmissions = NULL;
      pProgressions->A0 = 0.0;

      if (pState->isValid())
        {
          mId2State[pState->getId()] = pState;
        }
      else
        {
          CLogger::error() << "Model: Invalid state for item '" << i << "'.";
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
          if (mPossibleTransmissions[itTrans->getEntryState() - mStates].Transmissions == NULL)
            {
              mPossibleTransmissions[itTrans->getEntryState() - mStates].Transmissions = new CTransmission*[mStateCount];
              memset(mPossibleTransmissions[itTrans->getEntryState() - mStates].Transmissions, 0, sizeof(void *) * mStateCount);
            }

          mPossibleTransmissions[itTrans->getEntryState() - mStates].Transmissions[itTrans->getContactState() - mStates] = &*itTrans;
        }
      else
        {
          CLogger::error() << "Model: Invalid transmission for item '" << i << "'.";
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
          mPossibleProgressions[itProg->getEntryState() - mStates].A0 += itProg->getPropability();
          mPossibleProgressions[itProg->getEntryState() - mStates].Progressions.push_back(&*itProg);
        }
      else
        {
          CLogger::error() << "Model: Invalid transition for item '" << i << "'.";
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
CModel::state_t CModel::StateToType(const CHealthState * pState)
{
  return INSTANCE->stateToType(pState);
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

  CRandom::uniform_real Uniform01(0.0, 1.0);

  CNode * pNode = CNetwork::Context.Active().beginNode();
  CNode * pNodeEnd = CNetwork::Context.Active().endNode();

  CTransmission ** pPossibleTransmissions = NULL;
  
  for (pNode = CNetwork::Context.Active().beginNode(); pNode != pNodeEnd; ++pNode)
    if (pNode->susceptibility > 0.0
        && (pPossibleTransmissions = mPossibleTransmissions[pNode->healthState].Transmissions) != NULL)
      {
        CEdge * pEdge = pNode->Edges;
        CEdge * pEdgeEnd = pNode->Edges + pNode->EdgesSize;

        CTransmission * pTransmission = NULL;

        struct Candidate
        {
          const CEdge * pEdge;
          const CTransmission * pTransmission;
          double Propensity;
        };

        std::vector< Candidate > Candidates;
        double A0 = 0.0;

        for (; pEdge != pEdgeEnd; ++pEdge)
          {
            if (pEdge->active
                && pEdge->pSource->infectivity > 0.0
                && (pTransmission = pPossibleTransmissions[pEdge->pSource->healthState]) != NULL)
              {
                Candidate Candidate;
                Candidate.pEdge = pEdge;
                Candidate.pTransmission = pTransmission;
                Candidate.Propensity = (*CTransmissionPropensity::pPROPENSITY)(pEdge, pTransmission);

                if (Candidate.Propensity > 0.0)
                  {
                    A0 += Candidate.Propensity;
                    Candidates.push_back(Candidate);
                  }
              }
          }

        if (A0 > 0.0
            && -log(Uniform01(CRandom::G.Active())) < A0 * *mpTransmissibility / 86400.0)
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
                CLogger::error() << "CModel:: Failed to create transmission for '" << pNode->id << "'.";
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
  const CProgression * pProgression = nextProgression(pNode->healthState);

  if (pProgression != NULL)
    {
      try
        {
          CActionQueue::addAction(pProgression->getDwellTime(), new CProgressionAction(pProgression, pNode));
        }
      catch (...)
        {
          CLogger::error() << "CModel:: Failed to create progression for '" << pNode->id << "'.";
        }
    }
}

// static
const CProgression * CModel::NextProgression(const CModel::state_t & state)
{
  return INSTANCE->nextProgression(state);
}

const CProgression * CModel::nextProgression(const CModel::state_t & state) const
{
  PossibleProgressions & Progressions = mPossibleProgressions[state];

  if (Progressions.A0 > 0.0)
    {
      double alpha = CRandom::uniform_real(0.0, Progressions.A0)(CRandom::G.Active());

      std::vector< const CProgression * >::const_iterator it = Progressions.Progressions.begin();
      std::vector< const CProgression * >::const_iterator end = Progressions.Progressions.end();

      for (; it != end; ++it)
        {
          alpha -= (*it)->getPropability();

          if (alpha < 0.0)
            break;
        }

      return (it != end) ? *it : *Progressions.Progressions.rbegin();
    }

  return NULL;
}

const CHealthState * CModel::getStates() const
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

CModel::state_t CModel::stateToType(const CHealthState * pState) const
{
  return  pState - mStates;
}

CHealthState * CModel::stateFromType(const CModel::state_t & type) const
{
  return mStates + type;
}

// static
const std::vector< CTransmission > & CModel::GetTransmissions()
{
  return INSTANCE->getTransmissions();
}

// static
int CModel::UpdateGlobalStateCounts()
{
  size_t Size = INSTANCE->mStateCount;
  CHealthState::Counts LocalStateCounts[Size];

  CHealthState * pState = INSTANCE->mStates;
  CHealthState * pStateEnd = pState + Size;
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
  size_t Size = INSTANCE->mStateCount;

  CHealthState::Counts Increment;
  CHealthState * pState = INSTANCE->mStates;
  CHealthState * pStateEnd = pState + Size;

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
          size_t Size = INSTANCE->mStateCount;
          CHealthState * pState = INSTANCE->mStates;
          CHealthState * pStateEnd = pState + Size;
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
            out << "," << (*it)->getId() << (((*it)->getType() == CVariable::Type::global) ? "(g)" : "(l)");

          out << std::endl;
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
void CModel::WriteGlobalStateCounts()
{
  if (CCommunicate::MPIRank == 0)
    {
      std::ofstream out;

      out.open(CSimConfig::getSummaryOutput().c_str(), std::ios_base::app);

      if (out.good())
        {
          size_t Size = INSTANCE->mStateCount;
          CHealthState * pState = INSTANCE->mStates;
          CHealthState * pStateEnd = pState + Size;

          out << CActionQueue::getCurrentTick();

          // Loop through all states
          for (; pState != pStateEnd; ++pState)
            {
              const CHealthState::Counts & Counts = pState->getGlobalCounts();

              out << "," << Counts.Current << "," << Counts.In << "," << Counts.Out;
            }

          // We also add variables
          CVariableList::const_iterator it = CVariableList::INSTANCE.begin();
          CVariableList::const_iterator end = CVariableList::INSTANCE.end();

          for (; it != end; ++it)
            {
              (*it)->getValue();
              out << "," << (*it)->toNumber();
            }

          out << std::endl;
        }
      else
        {
          CLogger::error("Error (Rank 0): Failed to open file '" + CSimConfig::getSummaryOutput() + "'.");
          exit(EXIT_FAILURE);
        }

      out.close();
    }
}
