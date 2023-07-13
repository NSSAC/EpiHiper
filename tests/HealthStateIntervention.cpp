#include "catch.hpp"

#include "utilities/CLogger.h"
#include "diseaseModel/CModel.h"
#include "intervention/CIntervention.h"
#include "intervention/CTrigger.h"
#include "actions/CActionQueue.h"
#include "actions/CChanges.h"
#include "math/CDependencyGraph.h"
#include "math/CSizeOf.h"
#include "sets/CSetReference.h"

extern std::string getAbsolutePath(const std::string & fileName);
extern void clearTest();

TEST_CASE("Health State Intervention", "[EpiHiper]")
{
  CModel::Load(getAbsolutePath("example/diseaseModel.json"));
  REQUIRE_FALSE(CLogger::hasErrors());

  CIntervention::load(getAbsolutePath("tests/data/HealthStateIntervention.json"));
  REQUIRE_FALSE(CLogger::hasErrors());

  REQUIRE(CSetReference::resolve());

  CActionQueue::init(0);
  CChanges::setCurrentTick(0);
  CLogger::updateTick();
  CDependencyGraph::buildGraph();

  REQUIRE(CTrigger::processAll());
  REQUIRE(CIntervention::processAll());
  REQUIRE(CActionQueue::processCurrentActions());

  REQUIRE(CModel::GetState("S")->getSusceptibility() == 0.5);
  REQUIRE(CModel::GetState("I")->getInfectivity() == 2.0);

  clearTest();  
}