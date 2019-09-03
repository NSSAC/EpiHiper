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

#include "diseaseModel/CModel.h"
#include "diseaseModel/CHealthState.h"
#include "diseaseModel/CProgression.h"
#include "diseaseModel/CTransmission.h"
#include "SimConfig.h"
#include "actions/CActionQueue.h"
#include "network/CEdge.h"
#include "network/CNetwork.h"
#include "network/CNode.h"
#include "utilities/CRandom.h"

// static
CModel * CModel::INSTANCE(NULL);

// static
void CModel::load(const std::string & modelFile)
{
  if (INSTANCE == NULL)
    {
      INSTANCE = new CModel(modelFile);
    }
}

// static
void CModel::release()
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
  , mId2State()
  , mpInitialState(NULL)
  , mTransmissions()
  , mProgressions()
  , mPossibleTransmissions()
  , mPossibleProgressions()
  , mValid(false)
{
  json_t * pRoot = SimConfig::loadJson(modelFile, JSON_DECODE_INT_AS_REAL);

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
      delete [] mStates;
    }
}

void CModel::fromJSON(const json_t * json)
{
  mValid = true;

  json_t * pValue = json_object_get(json, "states");
  mStates = new CHealthState[json_array_size(pValue)];
  CHealthState * pState = mStates;

  for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i, ++pState)
    {
      pState->fromJSON(json_array_get(pValue, i));

      if (pState->isValid())
        {
          mId2State[pState->getId()] = pState;
        }
      else
        {
          mValid = false;
        }
    }

  pValue = json_object_get(json, "initialState");

  if (json_is_string(pValue))
    {
      std::map< std::string, CHealthState * >::const_iterator found = mId2State.find(json_string_value(pValue));

      if (found != mId2State.end())
        {
          mpInitialState = found->second;
        }
    }

  mValid &= (mpInitialState != NULL);

  pValue = json_object_get(json, "transmissions");

  mTransmissions.resize(json_array_size(pValue));
  std::vector< CTransmission >::iterator itTrans = mTransmissions.begin();
  std::vector< CTransmission >::iterator endTrans = mTransmissions.end();

  for (size_t i = 0; itTrans != endTrans; ++itTrans, ++i)
    {
      itTrans->fromJSON(json_array_get(pValue, i), mId2State);
      mValid &= itTrans->isValid();
      mPossibleTransmissions[itTrans->getEntryState()][itTrans->getContactState()] = &*itTrans;
    }

  pValue = json_object_get(json, "transitions");

  mProgressions.resize(json_array_size(pValue));
  std::vector< CProgression >::iterator itProg = mProgressions.begin();
  std::vector< CProgression >::iterator endProg = mProgressions.end();

  for (size_t i = 0; itProg != endProg; ++itProg, ++i)
    {
      itProg->fromJSON(json_array_get(pValue, i), mId2State);
      mValid &= itProg->isValid();
      mPossibleProgressions[itProg->getEntryState()].push_back(&*itProg);
    }

  CAnnotation::fromJSON(json);
}

// static
const CHealthState & CModel::getInitialState()
{
  return *INSTANCE->mpInitialState;
}

// static
CHealthState * CModel::getState(const std::string & id)
{
  std::map< std::string, CHealthState * >::const_iterator found = INSTANCE->mId2State.find(id);

  if (found != INSTANCE->mId2State.end())
    return found->second;

  return NULL;
}

// static
CModel::state_t CModel::stateToType(const CHealthState * pState)
{
  return pState - INSTANCE->mStates;
}

// static
CHealthState * CModel::stateFromType(const CModel::state_t & type)
{
  return INSTANCE->mStates + type;
}

// static
const bool & CModel::isValid()
{
  return INSTANCE->mValid;
}

// static
bool CModel::processTransmissions()
{
  return INSTANCE->_processTransmissions();
}

bool CModel::_processTransmissions() const
{
  bool success = true;

  CRandom::uniform_real Uniform01(0.0, 1.0);

  CNode * pNode = CNetwork::INSTANCE->beginNode();
  CNode * pNodeEnd = CNetwork::INSTANCE->endNode();

  std::map< const CHealthState *, std::map< const CHealthState *,  const CTransmission * > >::const_iterator EntryStateFound;
  std::map< const CHealthState *, std::map< const CHealthState *,  const CTransmission * > >::const_iterator EntryStateNotFound = mPossibleTransmissions.end();

  for (pNode = CNetwork::INSTANCE->beginNode(); pNode  != pNodeEnd; ++pNode)
    if (pNode->susceptibility > 0.0 &&
        (EntryStateFound = mPossibleTransmissions.find(pNode->getHealthState())) != EntryStateNotFound)
      {
        CEdge * pEdge = pNode->Edges;
        CEdge * pEdgeEnd = pNode->Edges + pNode->EdgesSize;

        std::map< const CHealthState *,  const CTransmission * >::const_iterator ContactStateFound;
        std::map< const CHealthState *,  const CTransmission * >::const_iterator ContactStateNotFound = EntryStateFound->second.end();

        struct Candidate { const CNode * pContact; const CTransmission * pTransmission; double Propensity; };
        std::vector< Candidate > Candidates;
        double A0 = 0.0;

        for (; pEdge != pEdgeEnd; ++pEdge)
          {
            if (pEdge->active &&
                pEdge->pSource->infectivity > 0.0 &&
                (ContactStateFound = EntryStateFound->second.find(pEdge->pSource->getHealthState())) != ContactStateNotFound)
              {
                // ρ(P, P', Τi,j,k) = (| contactTime(P, P') ∩ [tn, tn + Δtn] |) × contactWeight(P, P') × σ(P, Χi) × ι(P',Χk) × ω(Τi,j,k)
                Candidate Candidate;
                Candidate.pContact = pEdge->pSource;
                Candidate.pTransmission = ContactStateFound->second;
                Candidate.Propensity = pEdge->duration * pEdge->weight * pNode->susceptibility
                    * Candidate.pContact->infectivity * Candidate.pTransmission->getTransmissibility();

                if (Candidate.Propensity > 0.0)
                  {
                    A0 += Candidate.Propensity;
                    Candidates.push_back(Candidate);
                  }
              }
          }

        if (A0 > 0 &&
            -log(Uniform01(CRandom::G)) < A0/86400)
          {
            double alpha = Uniform01(CRandom::G) * A0;

            std::vector< Candidate >::const_iterator itCandidate = Candidates.begin();
            std::vector< Candidate >::const_iterator endCandidate = Candidates.end();

            for (; itCandidate != endCandidate; ++itCandidate)
              {
                alpha -= itCandidate->Propensity;

                if (alpha < 0.0) break;
              }

            const Candidate & Candidate = (itCandidate != endCandidate) ? *itCandidate : *Candidates.rbegin();

            CCondition::CComparison< const CHealthState * > CheckState(CConditionDefinition::ComparisonType::Equal, &pNode->getHealthState(), pNode->getHealthState());

            CMetadata Info("StateChange", true);
            Info.set("ContactNode", (int) Candidate.pContact->id);

            CAction ChangeState(1.0, CheckState, Info);
            ChangeState.addOperation(new COperationInstance<CNode, const CTransmission *>(pNode, Candidate.pTransmission, NULL, &CNode::set));

            CActionQueue::addAction(0, ChangeState);
          }
      }

  return success;
}

// static
void CModel::stateChanged(CNode * pNode)
{
  INSTANCE->_stateChanged(pNode);
}

void CModel::_stateChanged(CNode * pNode) const
{

  // std::cout << pNode->id << ": " << pNode->Edges << ", " << pNode->Edges + pNode->EdgesSize << ", " << pNode->EdgesSize << std::endl;

  std::map< const CHealthState *, std::vector< const CProgression * > >::const_iterator EntryStateFound = mPossibleProgressions.find(pNode->getHealthState());

  if (EntryStateFound == mPossibleProgressions.end()) return;

  std::vector< const CProgression * >::const_iterator it = EntryStateFound->second.begin();
  std::vector< const CProgression * >::const_iterator end = EntryStateFound->second.end();

  double A0 = 0.0;

  for (; it != end; ++it)
    {
      A0 += (*it)->getPropability();
    }

  if (A0 > 0)
    {
      double alpha = CRandom::uniform_real(0.0, A0)(CRandom::G);

      for (it = EntryStateFound->second.begin(); it != end; ++it)
        {
          alpha -= (*it)->getPropability();

          if (alpha < 0.0) break;
        }

      const CProgression * pProgression = (it != end) ? *it : *EntryStateFound->second.rbegin();

      CCondition::CComparison< const CHealthState * > CheckState(CConditionDefinition::ComparisonType::Equal, &pNode->getHealthState(), pNode->getHealthState());

      CMetadata Info("StateChange", true);
      CAction ChangeState(1.0, CheckState, Info);
      ChangeState.addOperation(new COperationInstance<CNode, const CProgression *>(pNode, pProgression, NULL, &CNode::set));

      CActionQueue::addAction(pProgression->getDwellTime(), ChangeState);
    }
}

// static
const std::vector< CTransmission > & CModel::getTransmissions()
{
  return INSTANCE->mTransmissions;
}

// static
int CModel::updateGlobalStateCounts()
{
  size_t Size = INSTANCE->mId2State.size();
  size_t LocalStateCounts[Size];

  CHealthState * pState = INSTANCE->mStates;
  CHealthState * pStateEnd = pState + Size;
  size_t * pLocalCount = LocalStateCounts;

  for (; pState != pStateEnd; ++pState, ++pLocalCount)
    {
      *pLocalCount = pState->getLocalCount();
      pState->resetGlobalCount();
    }

  CCommunicate::Receive ReceiveCounts(&CModel::receiveGlobalStateCounts);
  CCommunicate::broadcast(LocalStateCounts, Size * sizeof(size_t), &ReceiveCounts);

  return (int) CCommunicate::ErrorCode::Success;
}


// static
CCommunicate::ErrorCode CModel::receiveGlobalStateCounts(std::istream & is, int sender)
{
  size_t Size = INSTANCE->mId2State.size();

  size_t Increment;
  CHealthState * pState = INSTANCE->mStates;
  CHealthState * pStateEnd = pState + Size;

  for (; pState != pStateEnd; ++pState)
    {
      is.read(reinterpret_cast<char *>(&Increment), sizeof(size_t));
      pState->incrementGlobalCount(Increment);
    }

  return CCommunicate::ErrorCode::Success;
}

