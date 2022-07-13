// BEGIN: Copyright 
// Copyright (C) 2020 - 2022 Rector and Visitors of the University of Virginia 
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

#include "utilities/CSimulation.h"
#include "utilities/CLogger.h"
#include "actions/CActionQueue.h"
#include "actions/CChanges.h"
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
CSimulation::CSimulation()
  : valid(false)
  , startTick(0)
  , endTick(0)
{
  startTick = CSimConfig::getStartTick();
  endTick = CSimConfig::getEndTick();
}

CSimulation::~CSimulation()
{}

// check all required components are ready
bool CSimulation::validate()
{
  valid = (startTick <= endTick);

  return valid;
}

bool CSimulation::run()
{
  bool success = true;
  std::chrono::time_point<std::chrono::steady_clock> Start = std::chrono::steady_clock::now();

  CActionQueue::setCurrentTick(startTick - 1);
  CChanges::setCurrentTick(startTick - 1);
  CChanges::initDefaultOutput();
  CModel::InitGlobalStateCountOutput();
  CDependencyGraph::buildGraph();

#pragma omp parallel
  {
    if (!CDependencyGraph::applyUpdateSequence())
#pragma omp atomic
      success &= false;

    if (!CInitialization::processAll())
#pragma omp atomic
      success &= false;

    CCommunicate::memUsage(CActionQueue::getCurrentTick());

    if (!CActionQueue::processCurrentActions())
#pragma omp atomic
      success &= false;
  }

  CLogger::info() << "CSimulation::initialization: duration = '" << std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000  << "' ms.";
  Start = std::chrono::steady_clock::now();

  CActionQueue::incrementTick();
  CChanges::incrementTick();

  CChanges::writeDefaultOutput();
  CModel::UpdateGlobalStateCounts();
  CModel::WriteGlobalStateCounts();

  CLogger::info() << "CSimulation::output: duration = '" << std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000  << "' ms.";
  Start = std::chrono::steady_clock::now();

  CNetwork::Context.Master().broadcastChanges();

  CLogger::info() << "CSimulation::synchronize: duration = '" << std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000  << "' ms.";

  CNetwork::dumpActiveNetwork();

  CStatus::update("running", (100.0 * std::max((CActionQueue::getCurrentTick() - CSimConfig::getStartTick() + 1), 0)) / (CSimConfig::getEndTick() - CSimConfig::getStartTick() + 1));

  while (CActionQueue::getCurrentTick() < endTick && success)
    {
#pragma omp parallel
      {
        std::chrono::time_point<std::chrono::steady_clock> Start = std::chrono::steady_clock::now();
        CVariableList::INSTANCE.resetAll();

        if (!CDependencyGraph::applyUpdateSequence())
#pragma omp atomic
          success &= false;

        CLogger::info() << "CSimulation::applyUpdateSequence: duration = '" << std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000  << "' ms.";
        Start = std::chrono::steady_clock::now();

        CModel::ProcessTransmissions();

        CLogger::info() << "CSimulation::ProcessTransmissions: duration = '" << std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000  << "' ms.";
        Start = std::chrono::steady_clock::now();

        if (!CTrigger::processAll())
#pragma omp atomic
          success &= false;

        if (!CIntervention::processAll())
#pragma omp atomic
          success &= false;

        CLogger::info() << "CSimulation::ProcessIntervention: duration = '" << std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000  << "' ms.";
        Start = std::chrono::steady_clock::now();

        CCommunicate::memUsage(CActionQueue::getCurrentTick());

        if (!CActionQueue::processCurrentActions())
#pragma omp atomic
          success &= false;

        CLogger::info() << "CSimulation::processCurrentActions: duration = '" << std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000  << "' ms.";
      }

      Start = std::chrono::steady_clock::now();
      CActionQueue::incrementTick();
      CChanges::incrementTick();

      CChanges::writeDefaultOutput();
      CModel::UpdateGlobalStateCounts();
      CModel::WriteGlobalStateCounts();

      CLogger::info() << "CSimulation::output: duration = '" << std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000  << "' ms.";
      Start = std::chrono::steady_clock::now();

      CNetwork::Context.Master().broadcastChanges();

      CLogger::info() << "CSimulation::synchronize: duration = '" << std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000  << "' ms.";

      CNetwork::dumpActiveNetwork();

      CStatus::update("running", (100.0 * std::max((CActionQueue::getCurrentTick() - CSimConfig::getStartTick() + 1), 0)) / (CSimConfig::getEndTick() - CSimConfig::getStartTick() + 1));

      std::map< int, size_t >::const_iterator found = CSimConfig::getReseed().find(CActionQueue::getCurrentTick());

      if (found != CSimConfig::getReseed().end())
#pragma omp master
        CRandom::init(found->second);

    }

  return success;
}
