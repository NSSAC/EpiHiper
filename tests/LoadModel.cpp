#include "catch.hpp"

#include "diseaseModel/CModel.h"
#include "utilities/CLogger.h"

extern std::string getAbsolutePath(const std::string & fileName);
extern void clearTest();

TEST_CASE("Load model", "[EpiHiper]")
{
  CModel::Load(getAbsolutePath("example/diseaseModel.json"));

  REQUIRE_FALSE(CLogger::hasErrors());

  clearTest();
}