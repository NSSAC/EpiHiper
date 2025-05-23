// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2024 Rector and Visitors of the University of Virginia 
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

#include "catch.hpp"

#include "utilities/CLogger.h"
#include "diseaseModel/CModel.h"
#include "intervention/CInitialization.h"
#include "actions/CActionQueue.h"
#include "actions/CChanges.h"
#include "math/CDependencyGraph.h"
#include "math/CSizeOf.h"
#include "sets/CSetReference.h"
#include "sets/CSetList.h"
#include "network/CNetwork.h"
#include "traits/CTrait.h"
#include "utilities/CSimConfig.h"
#include "variables/CVariable.h"
#include "variables/CVariableList.h"

extern std::string getAbsolutePath(const std::string & fileName);
extern void clearTest();

TEST_CASE("Isolation", "[EpiHiper]")
{
  CSimConfig::init();
  CTrait::init();
  CLogger::pushLevel(CLogger::LogLevel::debug);
  CLogger::info("Starting Test: Isolation");

  CNetwork::init(getAbsolutePath("example/contactNetwork.txt"));
  REQUIRE_FALSE(CLogger::hasErrors());

  CModel::Load(getAbsolutePath("example/diseaseModel.json"));
  REQUIRE_FALSE(CLogger::hasErrors());

  CInitialization::load(getAbsolutePath("tests/data/Isolation.json"));
  REQUIRE_FALSE(CLogger::hasErrors());

  REQUIRE(CSetReference::resolve());
  REQUIRE(CCommunicate::allocateRMA() == (int) CCommunicate::ErrorCode::Success);

  CNetwork::Context.Master().load();

  CActionQueue::init(0);
  CChanges::setCurrentTick(0);
  CLogger::updateTick();
  CDependencyGraph::buildGraph();

#pragma omp parallel
  {
    CVariableList::INSTANCE.resetAll(true);

    REQUIRE(CDependencyGraph::applyComputeOnceOrder());
    REQUIRE(CDependencyGraph::applyUpdateOrder());
    REQUIRE(CInitialization::processAll());
    REQUIRE(CActionQueue::processCurrentActions());
  }

  REQUIRE(CSizeOf::Set("population") == 6);
  REQUIRE(CSizeOf::Set("edge_target_in_population") == 64);
  REQUIRE(CSizeOf::Set("edge_target_not_in_population") == 152);
  REQUIRE(CSizeOf::Set("edge_source_in_population") == 64);
  REQUIRE(CSizeOf::Set("edge_source_not_in_population") == 152);
  REQUIRE(CSizeOf::Set("edge_to_disable") == 68);

  REQUIRE(CVariableList::INSTANCE["edges_cut"].toValue().toNumber() == 68);
  CLogger::popLevel();
  clearTest();  
}