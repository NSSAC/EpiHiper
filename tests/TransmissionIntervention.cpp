#include "catch.hpp"

#include "utilities/CLogger.h"
#include "diseaseModel/CModel.h"
#include "diseaseModel/CTransmission.h"
#include "intervention/CIntervention.h"
#include "intervention/CTrigger.h"
#include "actions/CActionQueue.h"
#include "actions/CChanges.h"
#include "math/CDependencyGraph.h"
#include "math/CSizeOf.h"
#include "sets/CSetReference.h"

extern std::string getAbsolutePath(const std::string & fileName);
extern void clearTest();

TEST_CASE("Transmission Intervention", "[EpiHiper]")
{
  CModel::Load(getAbsolutePath("example/diseaseModel.json"));
  REQUIRE_FALSE(CLogger::hasErrors());

  CIntervention::load(getAbsolutePath("tests/data/TransmissionIntervention.json"));
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

  REQUIRE(CModel::GetTransmission("infection_I_wanedS")->getTransmissibility() == 0.5);
  REQUIRE(CModel::GetTransmission("infection_I_wanedV")->getInfectivityFactorOperation() == "{\"operator\":\"*=\",\"value\":2.0}");
  REQUIRE(CModel::GetTransmission("infection_I_V")->getSusceptibilityFactorOperation() == "{\"operator\":\"=\",\"value\":0.5}");

  CActionQueue::incrementTick();
  CChanges::incrementTick();
  CLogger::updateTick();

  REQUIRE(CDependencyGraph::applyUpdateOrder());
  REQUIRE(CTrigger::processAll());
  REQUIRE(CIntervention::processAll());
  REQUIRE(CActionQueue::processCurrentActions());

  REQUIRE(CModel::GetTransmission("infection_I_V")->getSusceptibilityFactorOperation() == "{}");

  clearTest();  
}