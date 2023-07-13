#include "catch.hpp"

#include "utilities/CLogger.h"
#include "diseaseModel/CModel.h"
#include "diseaseModel/CProgression.h"
#include "intervention/CIntervention.h"
#include "intervention/CTrigger.h"
#include "actions/CActionQueue.h"
#include "actions/CChanges.h"
#include "math/CDependencyGraph.h"
#include "math/CSizeOf.h"
#include "sets/CSetReference.h"

extern std::string getAbsolutePath(const std::string & fileName);
extern void clearTest();

TEST_CASE("Progression Intervention", "[EpiHiper]")
{
  CModel::Load(getAbsolutePath("example/diseaseModel.json"));
  REQUIRE_FALSE(CLogger::hasErrors());

  CIntervention::load(getAbsolutePath("tests/data/ProgressionIntervention.json"));
  REQUIRE_FALSE(CLogger::hasErrors());

  REQUIRE(CSetReference::resolve());

  CActionQueue::init(0);
  CChanges::setCurrentTick(0);
  CLogger::updateTick();
  CDependencyGraph::buildGraph();

  REQUIRE(CDependencyGraph::applyComputeOnceOrder());
  REQUIRE(CDependencyGraph::applyUpdateOrder());
  REQUIRE(CTrigger::processAll());
  REQUIRE(CIntervention::processAll());
  REQUIRE(CActionQueue::processCurrentActions());

  REQUIRE(CModel::GetProgression("I_to_R")->getPropensity()== 2.0);
  REQUIRE(CModel::GetProgression("R_to_wanedS")->getInfectivityFactorOperation() == "{\"operator\":\"*=\",\"value\":2.0}");
  REQUIRE(CModel::GetProgression("V_to_wanedV")->getDwellTime() == "{\"normal\":{\"mean\":2.0,\"standardDeviation\":1.0}}");

  CActionQueue::incrementTick();
  CChanges::incrementTick();
  CLogger::updateTick();

  REQUIRE(CDependencyGraph::applyUpdateOrder());
  REQUIRE(CTrigger::processAll());
  REQUIRE(CIntervention::processAll());
  REQUIRE(CActionQueue::processCurrentActions());

  REQUIRE(CModel::GetProgression("R_to_wanedS")->getInfectivityFactorOperation() == "{}");
  REQUIRE(CModel::GetProgression("V_to_wanedV")->getDwellTime() == "{\"discrete\":[{\"probability\":0.2,\"value\":1.0},{\"probability\":0.6,\"value\":2.0},{\"probability\":0.2,\"value\":3.0}]}");
  
  clearTest();  
}