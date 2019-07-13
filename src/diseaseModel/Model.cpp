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

#include "SimConfig.h"
#include "diseaseModel/State.h"
#include "diseaseModel/Transmission.h"
#include "diseaseModel/Progression.h"
#include "diseaseModel/Model.h"

#include "network/Network.h"
#include "network/Node.h"
#include "network/Edge.h"

#include "utilities/Random.h"

#include "actions/ActionQueue.h"

// static
Model * Model::INSTANCE(NULL);

// static
void Model::load(const std::string & modelFile)
{
  if (INSTANCE == NULL)
    {
      INSTANCE = new Model(modelFile);
    }
}

// static
void Model::release()
{
  if (INSTANCE != NULL)
    {
      delete INSTANCE;
      INSTANCE = NULL;
    }
}


Model::Model(const std::string & modelFile)
  : Annotation()
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
      fromJSON(pRoot );
    }

  json_decref(pRoot);
}

// virtual
Model::~Model()
{
  if (mStates != NULL)
    {
      delete [] mStates;
    }
}

void Model::fromJSON(const json_t * json)
{
  mValid = true;

  json_t * pValue = json_object_get(json, "states");
  mStates = new State[json_array_size(pValue)];
  State * pState = mStates;

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
      std::map< std::string, State * >::const_iterator found = mId2State.find(json_string_value(pValue));

      if (found != mId2State.end())
        {
          mpInitialState = found->second;
        }
    }

  mValid &= (mpInitialState != NULL);

  pValue = json_object_get(json, "transmissions");

  mTransmissions.resize(json_array_size(pValue));
  std::vector< Transmission >::iterator itTrans = mTransmissions.begin();
  std::vector< Transmission >::iterator endTrans = mTransmissions.end();

  for (size_t i = 0; itTrans != endTrans; ++itTrans, ++i)
    {
      itTrans->fromJSON(json_array_get(pValue, i), mId2State);
      mValid &= itTrans->isValid();
      mPossibleTransmissions[itTrans->getEntryState()][itTrans->getContactState()] = &*itTrans;
    }

  pValue = json_object_get(json, "transitions");

  mProgressions.resize(json_array_size(pValue));
  std::vector< Progression >::iterator itProg = mProgressions.begin();
  std::vector< Progression >::iterator endProg = mProgressions.end();

  for (size_t i = 0; itProg != endProg; ++itProg, ++i)
    {
      itProg->fromJSON(json_array_get(pValue, i), mId2State);
      mValid &= itProg->isValid();
      mPossibleProgressions[itProg->getEntryState()].push_back(&*itProg);
    }

  Annotation::fromJSON(json);
}

// static
const State & Model::getInitialState()
{
  return *INSTANCE->mpInitialState;
}

// static
Model::state_t Model::stateToType(const State * pState)
{
  return pState - INSTANCE->mStates;
}

// static
State * Model::stateFromType(const Model::state_t & type)
{
  return INSTANCE->mStates + type;
}

// static
const bool & Model::isValid()
{
  return INSTANCE->mValid;
}

// static
bool Model::processTransmissions()
{
  return INSTANCE->_processTransmissions();
}

bool Model::_processTransmissions() const
{
  bool success = true;

  Random::uniform_real Uniform01(0.0, 1.0);

  NodeData * pNode = Network::INSTANCE->beginNode();
  NodeData * pNodeEnd = Network::INSTANCE->endNode();

  std::map< const State *, std::map< const State *,  const Transmission * > >::const_iterator EntryStateFound;
  std::map< const State *, std::map< const State *,  const Transmission * > >::const_iterator EntryStateNotFound = mPossibleTransmissions.end();

  for (pNode = Network::INSTANCE->beginNode(); pNode  != pNodeEnd; ++pNode)
    if (pNode->susceptibility > 0.0 &&
        (EntryStateFound = mPossibleTransmissions.find(pNode->pHealthState)) != EntryStateNotFound)
      {
        EdgeData * pEdge = pNode->Edges;
        EdgeData * pEdgeEnd = pNode->Edges + pNode->EdgesSize;

        std::map< const State *,  const Transmission * >::const_iterator ContactStateFound;
        std::map< const State *,  const Transmission * >::const_iterator ContactStateNotFound = EntryStateFound->second.end();

        struct Candidate { const NodeData * pContact; const Transmission * pTransmission; double Propensity; };
        std::vector< Candidate > Candidates;
        double A0 = 0.0;

        for (; pEdge != pEdgeEnd; ++pEdge)
          {
            if (pEdge->active &&
                pEdge->pSource->infectivity > 0.0 &&
                (ContactStateFound = EntryStateFound->second.find(pEdge->pSource->pHealthState)) != ContactStateNotFound)
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
            -log(Uniform01(Random::G)) < A0/86400)
          {
            double alpha = Uniform01(Random::G) * A0;

            std::vector< Candidate >::const_iterator itCandidate = Candidates.begin();
            std::vector< Candidate >::const_iterator endCandidate = Candidates.end();

            for (; itCandidate != endCandidate; ++itCandidate)
              {
                alpha -= itCandidate->Propensity;

                if (alpha < 0.0) break;
              }

            const Candidate & Candidate = (itCandidate != endCandidate) ? *itCandidate : *Candidates.rbegin();

            Condition::Comparison< const State * > CheckState(Condition::ComparisonType::Equal, &pNode->pHealthState, pNode->pHealthState);

            Metadata Info("StateChange", true);
            Info.set("ContactNode", (int) Candidate.pContact->id);

            Action ChangeState(1.0, CheckState, Info);
            ChangeState.addOperation(OperationInstance<Node, const Transmission *>(Node(pNode), Candidate.pTransmission, &Node::set));

            ActionQueue::addAction(0, ChangeState);
          }
      }

  return success;
}

// static
void Model::stateChanged(NodeData * pNode)
{
  INSTANCE->_stateChanged(pNode);
}

void Model::_stateChanged(NodeData * pNode) const
{

  // std::cout << pNode->id << ": " << pNode->Edges << ", " << pNode->Edges + pNode->EdgesSize << ", " << pNode->EdgesSize << std::endl;

  std::map< const State *, std::vector< const Progression * > >::const_iterator EntryStateFound = mPossibleProgressions.find(pNode->pHealthState);

  if (EntryStateFound == mPossibleProgressions.end()) return;

  std::vector< const Progression * >::const_iterator it = EntryStateFound->second.begin();
  std::vector< const Progression * >::const_iterator end = EntryStateFound->second.end();

  double A0 = 0.0;

  for (; it != end; ++it)
    {
      A0 += (*it)->getPropability();
    }

  if (A0 > 0)
    {
      double alpha = Random::uniform_real(0.0, A0)(Random::G);

      for (it = EntryStateFound->second.begin(); it != end; ++it)
        {
          alpha -= (*it)->getPropability();

          if (alpha < 0.0) break;
        }

      const Progression * pProgression = (it != end) ? *it : *EntryStateFound->second.rbegin();

      Condition::Comparison< const State * > CheckState(Condition::ComparisonType::Equal, &pNode->pHealthState, pNode->pHealthState);

      Metadata Info("StateChange", true);
      Action ChangeState(1.0, CheckState, Info);
      ChangeState.addOperation(OperationInstance<Node, const Progression *>(Node(pNode), pProgression, &Node::set));

      ActionQueue::addAction(pProgression->getDwellTime(), ChangeState);
    }
}

// static
const std::vector< Transmission > & Model::getTransmissions()
{
  return INSTANCE->mTransmissions;
}
