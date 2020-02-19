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
}

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
