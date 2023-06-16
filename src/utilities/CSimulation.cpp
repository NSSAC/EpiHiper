// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2020 - 2023 Rector and Visitors of the University of Virginia 
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

  // Initialization is reported prior to the start tick of the simulation.
  CActionQueue::init(startTick - 1);
  CChanges::setCurrentTick(startTick - 1);
  CLogger::updateTick();
  CCommunicate::memUsage();

  CChanges::determineNodesRequested();
  CChanges::initDefaultOutput();
  CModel::InitGlobalStateCountOutput();
  CDependencyGraph::buildGraph();

#pragma omp parallel
  {
    CVariableList::INSTANCE.resetAll(true);

    if (!CDependencyGraph::applyComputeOnceOrder())
#pragma omp atomic
      success &= false;

    if (!CDependencyGraph::applyUpdateOrder())
#pragma omp atomic
      success &= false;

    if (!CInitialization::processAll())
#pragma omp atomic
      success &= false;

    CVariableList::INSTANCE.resetAll();

    if (!CActionQueue::processCurrentActions())
#pragma omp atomic
      success &= false;

    CVariableList::INSTANCE.synchronizeChangedVariables();
  }

  CLogger::info("CSimulation::initialization: duration = '{}' \xc2\xb5s.", std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000);

  Start = std::chrono::steady_clock::now();

  CActionQueue::incrementTick();
  CChanges::incrementTick();
  CLogger::updateTick();
  CCommunicate::memUsage();

  CChanges::writeDefaultOutput();
  CModel::UpdateGlobalStateCounts();
  CModel::WriteGlobalStateCounts();

  CLogger::info("CSimulation::output: duration = '{}' \xc2\xb5s.", std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000);
  Start = std::chrono::steady_clock::now();

  CNetwork::Context.Master().broadcastChanges();

  CLogger::info("CSimulation::synchronize: duration = '{}' \xc2\xb5s.", std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000);

  CNetwork::dumpActiveNetwork();

  CStatus::update("running", (100.0 * std::max((CActionQueue::getCurrentTick() - CSimConfig::getStartTick() + 1), 0)) / (CSimConfig::getEndTick() - CSimConfig::getStartTick() + 1));

  while (CActionQueue::getCurrentTick() < endTick && success)
    {
#pragma omp parallel
      {
        std::chrono::time_point<std::chrono::steady_clock> Start = std::chrono::steady_clock::now();

        if (!CDependencyGraph::applyUpdateOrder())
#pragma omp atomic
          success &= false;

        CLogger::info("CSimulation::applyUpdateOrder: duration = '{}' \xc2\xb5s.", std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000);

        Start = std::chrono::steady_clock::now();

        CModel::ProcessTransmissions();

        CLogger::info("CSimulation::ProcessTransmissions: duration = '{}' \xc2\xb5s.", std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000);

        Start = std::chrono::steady_clock::now();

        if (!CTrigger::processAll())
#pragma omp atomic
          success &= false;

        if (!CIntervention::processAll())
#pragma omp atomic
          success &= false;

        CLogger::info("CSimulation::ProcessIntervention: duration = '{}' \xc2\xb5s.", std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000);

        Start = std::chrono::steady_clock::now();

        CVariableList::INSTANCE.resetAll();

        if (!CActionQueue::processCurrentActions())
#pragma omp atomic
          success &= false;
        
        CLogger::info("CSimulation::processCurrentActions: duration = '{}' \xc2\xb5s.", std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000);

        CVariableList::INSTANCE.synchronizeChangedVariables();
      }

      Start = std::chrono::steady_clock::now();
      CActionQueue::incrementTick();
      CChanges::incrementTick();
      CLogger::updateTick();
      CCommunicate::memUsage();

      CChanges::writeDefaultOutput();
      CModel::UpdateGlobalStateCounts();
      CModel::WriteGlobalStateCounts();

      CLogger::info("CSimulation::output: duration = '{}' \xc2\xb5s.", std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000);
      Start = std::chrono::steady_clock::now();

      CNetwork::Context.Master().broadcastChanges();

      CLogger::info("CSimulation::synchronize: duration = '{}' \xc2\xb5s.", std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000);

      CNetwork::dumpActiveNetwork();

      CStatus::update("running", (100.0 * std::max((CActionQueue::getCurrentTick() - CSimConfig::getStartTick() + 1), 0)) / (CSimConfig::getEndTick() - CSimConfig::getStartTick() + 1));

      std::map< int, size_t >::const_iterator found = CSimConfig::getReseed().find(CActionQueue::getCurrentTick());

      if (found != CSimConfig::getReseed().end())
#pragma omp master
        CRandom::init(found->second);

    }

  return success;
}
