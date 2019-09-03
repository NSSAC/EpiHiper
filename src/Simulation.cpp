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

#include <set>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include "Simulation.h"

#include "actions/CActionQueue.h"
#include "diseaseModel/CModel.h"
#include "Misc.h"
#include "SimConfig.h"
#include "network/CEdge.h"
#include "network/CNetwork.h"
#include "network/CNode.h"
#include "utilities/CCommunicate.h"
#include "utilities/CRandom.h"

std::vector<DummyTransition> transitions;

// initialize according to config
Simulation::Simulation(int seed, std::string dbconn) {
  valid = false;
  randomSeed = seed;
  startTick = 0;
  endTick = -1;

  /*
  disease = NULL;
  network = NULL;
  actionQueue = NULL;
  compEnv = NULL;
  db = NULL;
  output = NULL;
  log = NULL;
  */

  startTick = SimConfig::getStartTick();
  endTick = SimConfig::getEndTick();
  networkFile = SimConfig::getContactNetwork();
  outputFile = SimConfig::getOutput();

  /*
  readDiseaseModel();
  partition();
  readNetwork();
  readInterventions();
  initializeNodes();
  */
}

Simulation::~Simulation() {
}

// check all required components are ready
void Simulation::validate() {
  valid = true;
  if (endTick < startTick) valid = false;
  /*
  if (disease == NULL) valid = false;
  if (network == NULL) valid = false;
  if (actionQueue == NULL) valid = false;
  if (compEnv == NULL) valid = false;
  */
  // db, output, log are not necessary in all simulations  
}

/*
void Simulation::run() {
  // process initialization
  for (std::vector<Intervention>::iterator init = initialization.begin();
       init != initialization.end(); ++init) {
    std::vector<Node> candidates = init->computeCandidates(network,population);
    for (std::vector<Node>::iterator node = candidates.begin();
	 node != candidates.end(); ++node) {
      // schedule action for node to actionQueue
    }
  }

  // loop
  int tick = startTick;
  while (tick <= endTick) {
    std::vector<Action> scheduledActions = actionQueue->getActions(tick);
    actionQueue->clearActions(tick);
    for (std::vector<Action>::iterator it = scheduledActions.begin();
	 it != scheduledActions.end(); ++it) {
      it->checkConditions();
      if (it->conditionsTrue()) {
	execute(*it);
      }
    }
    // broadcast and receive all changes
    compEnv->exchange(localChanges);
    // broadcast and receive non-local actions ???

    // write changes to output
    // one PE writes all changes to file
    // or each PE writes local changes to DB

    tick++;

    // process infections
    for (std::vector<Node>::iterator node = population.begin();
	 node != population.end(); ++node) {
      if (node->isLocal()) {
	nextState = node->computeInfection(disease,network,population);
	if (nextState != NULL) {
	  // schedule action to actionQueue
	  // condition = node->getCurrentState(); priority = 1
	}
      }
    }

    processInterventions();
  }
}

void Simulation::processInterventions() {
  for (std::vector<Intervention>::iterator intv = interventions.begin();
       intv != interventions.end(); ++intv) {
    std::vector<Node> candidates = intv->computeCandidates(network,population);
    for (std::vector<Node>::iterator node = candidates.begin();
	 node != candidates.end(); ++node) {
      // schedule action for node to actionQueue
    }
  }
}

Simulation::execute(Action action) {
  // execute assignment
  // scaling factors, traits, (de)activate edge, health state

  if (action.isStateTransition()) {
    // compute next state transition
    Node node = action.getTarget();
    Transition transition = disease.getTransition(node.currentState);
    health_t nextState = transition.nextState;
    int delay = transition.delay; // ticks to transition
    // schedule (node,nextState,delay,condition=currentState,priority=1)
    // to actionQueue
  }
  // cascading events
  processInterventions();
}

*/

void Simulation::test()
{
  /**
   * We just randomly pick nodes to be infective
   */
  CNode * pNodes = CNetwork::INSTANCE->beginNode();
  int NumberOfNodes = CNetwork::INSTANCE->endNode() - pNodes - 1;
  CRandom::uniform_int uniform(0, NumberOfNodes);

  const CTransmission & Symptomatic = *CModel::getTransmissions().begin();

  CActionQueue::setCurrentTick(startTick - 1);
  Changes::setCurrentTick(startTick - 1);
  Changes::initDefaultOutput();

  size_t perNode = std::round(100.0/CCommunicate::MPIProcesses);

  for (size_t i = 0; i < 100; ++i)
    {
      CNode * pNode = pNodes + uniform(CRandom::G);

      bool sussess = pNode->set(&Symptomatic, NULL, CMetadata());
    }

  CActionQueue::processCurrentActions();
  Changes::writeDefaultOutput();
  CModel::updateGlobalStateCounts();
  CNetwork::INSTANCE->broadcastChanges();
  CActionQueue::incrementTick();
  Changes::incrementTick();

  for (int tick = startTick; tick < endTick; ++tick)
    {
      CModel::processTransmissions();
      CActionQueue::processCurrentActions();
      Changes::writeDefaultOutput();
      CModel::updateGlobalStateCounts();
      CNetwork::INSTANCE->broadcastChanges();
      CActionQueue::incrementTick();
      Changes::incrementTick();
    }
}

void Simulation::dummyRun() {
  CCommunicate::Status status;

  std::set<personid_t> population;
  if (CCommunicate::MPIRank == 0) {
    std::ifstream f(networkFile.data());
    if (! f.good()) {
      std::cerr << "fail to read file " << networkFile << std::endl;
    }
    else {
      std::stringstream buffer;
      buffer << f.rdbuf();
      std::string edge;
      std::getline(buffer, edge); // skip json line
      std::getline(buffer, edge); // skip CSV header line
      netsize_t edgeCount = 0;
      while (std::getline(buffer, edge)) {
	edgeCount++;
	std::vector<std::string> items = split(edge,',');
	population.insert(std::stoull(items[0]));
	population.insert(std::stoull(items[2]));
      }
      // std::cout << "M = " << edgeCount << std::endl;
    }
    // std::cout << "N = " << population.size() << std::endl;
  }

  uint64_t N;
  uint64_t sizeSubpop[CCommunicate::MPIProcesses];
  if (CCommunicate::MPIRank == 0) {
    N = population.size();
    for (int i = 0; i < CCommunicate::MPIProcesses; i++) {
      sizeSubpop[i] = N / CCommunicate::MPIProcesses;
    }
    sizeSubpop[0] += (N - (N / CCommunicate::MPIProcesses) * CCommunicate::MPIProcesses);
  }

  CCommunicate::broadcast(sizeSubpop, CCommunicate::MPIProcesses, MPI_UINT64_T, 0);
  personid_t subpop[sizeSubpop[CCommunicate::MPIRank]];
  /*
  std::cout << "subpop size of rank " << Communicate::Rank << "=" << sizeSubpop[Communicate::Rank]
	    << std::endl;
  */

  if (CCommunicate::MPIRank == 0) {
    // divide population and send ids
    std::set<personid_t>::iterator iter = population.begin();
    personid_t *subpopTmp;
    for (int rank = 1; rank < CCommunicate::MPIProcesses; rank++) {
      subpopTmp = new personid_t[sizeSubpop[rank]];
      for (uint64_t index = 0; index < sizeSubpop[rank]; index++) {
	subpopTmp[index] = *iter;
	++iter;
      }
      CCommunicate::send(subpopTmp, sizeSubpop[rank], MPI_UINT64_T, rank, CCommunicate::MPIRank);
      delete [] subpopTmp;
    }
    for (uint64_t index = 0; index < sizeSubpop[CCommunicate::MPIRank]; index++) {
      if (iter != population.end()) {
	subpop[index] = *iter;
	++iter;
      }
      else {
	std::cerr << "pids running out" << std::endl;
      }
    }
  }
  else {
    CCommunicate::receive(subpop, sizeSubpop[CCommunicate::MPIRank], MPI_UINT64_T, 0, 0, &status);
  }

  int T = endTick - startTick + 1;
  N = sizeSubpop[CCommunicate::MPIRank];
  std::shuffle(&subpop[0], &subpop[N], CRandom::G);
  int totalInfection = 0;

  CRandom::uniform_int uniform;

  std::string contact;
  int tick = startTick;
  while (tick <= endTick) {
    int numInfection = int(N * exp(-(8.*tick/T-4)*(8.*tick/T-4)/2)/T);
    for (int i = 0; i < numInfection; i++) {
      if (totalInfection == 0) contact = "-1";
      else
        {
          contact = std::to_string(transitions[uniform(CRandom::G, CRandom::uniform_int_p(0, totalInfection - 1))].id);
        }
      DummyTransition t = {tick, subpop[totalInfection+i],
			   "infectious",contact};
      transitions.push_back(t);
    }
    totalInfection = transitions.size();
    tick++;
  }

  if (CCommunicate::MPIRank == 0)
    {
      std::ofstream out;

      out.open(outputFile.data());

      if (out.good())
        {
          out << "tick,pid,exit_state,contact_pid" << std::endl;
        }

      out.close();
    }

  CCommunicate::ClassMemberSequentialProcess< Simulation > WriteData(this, &Simulation::writedata);
  CCommunicate::sequential(0, &WriteData);
}

CCommunicate::ErrorCode Simulation::writedata()
{
  std::ofstream out;
  out.open(outputFile.data(), std::ios_base::app);

  if (out.fail()) return CCommunicate::ErrorCode::FileOpenError;

  for (std::vector<DummyTransition>::iterator it = transitions.begin(); it != transitions.end(); ++it)
    {
      out << it->tick << "," << it->id << "," << it->exitState << "," << it->idContact << std::endl;
    }

  out.close();

  return CCommunicate::ErrorCode::Success;
}
