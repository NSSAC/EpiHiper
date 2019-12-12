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
#include "intervention/CInitialization.h"
#include "intervention/CIntervention.h"
#include "intervention/CTrigger.h"
#include "math/CDependencyGraph.h"
#include "network/CEdge.h"
#include "network/CNetwork.h"
#include "network/CNode.h"
#include "utilities/CCommunicate.h"
#include "utilities/CRandom.h"
#include "utilities/CSimConfig.h"
#include "utilities/CStatus.h"
#include "variables/CVariableList.h"

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

  startTick = CSimConfig::getStartTick();
  endTick = CSimConfig::getEndTick();
  networkFile = CSimConfig::getContactNetwork();
  outputFile = CSimConfig::getOutput();

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

void Simulation::run()
{
  CActionQueue::setCurrentTick(startTick - 1);
  Changes::setCurrentTick(startTick - 1);
  Changes::initDefaultOutput();
  CModel::initGlobalStateCountOutput();

  CDependencyGraph::buildGraph();
  CDependencyGraph::applyUpdateSequence();

  CInitialization::processAll();
  CCommunicate::memUsage(CActionQueue::getCurrentTick());

  CActionQueue::processCurrentActions();
  CActionQueue::incrementTick();
  Changes::incrementTick();

  Changes::writeDefaultOutput();
  CModel::updateGlobalStateCounts();
  CModel::writeGlobalStateCounts();
  CNetwork::INSTANCE->broadcastChanges();

  CStatus::update("EpiHiper", "running", (100.0 * std::max((CActionQueue::getCurrentTick() - CSimConfig::getStartTick() + 1), 0)) / (CSimConfig::getEndTick() - CSimConfig::getStartTick() + 1));

  while (CActionQueue::getCurrentTick() < endTick)
    {
      CVariableList::INSTANCE.resetAll();
      CDependencyGraph::applyUpdateSequence();
      CModel::processTransmissions();
      CTrigger::processAll();
      CIntervention::processAll();
      CCommunicate::memUsage(CActionQueue::getCurrentTick());
      CActionQueue::processCurrentActions();

      CActionQueue::incrementTick();
      Changes::incrementTick();

      Changes::writeDefaultOutput();
      CModel::updateGlobalStateCounts();
      CModel::writeGlobalStateCounts();
      CNetwork::INSTANCE->broadcastChanges();

      CStatus::update("EpiHiper", "running", (100.0 * std::max((CActionQueue::getCurrentTick() - CSimConfig::getStartTick() + 1), 0)) / (CSimConfig::getEndTick() - CSimConfig::getStartTick() + 1));
    }
}
