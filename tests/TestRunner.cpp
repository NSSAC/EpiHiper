// Copyright (C) 2021 by Pedro Mendes, Rector and Visitors of the
// University of Virginia, University of Heidelberg, and University
// of Connecticut School of Medicine.
// All rights reserved.

/**
 * Main Entry point for all tests (defines catch main, and only one
 * test file can do it). This file is there to include utility
 * functions used in all other test files.
 */

// now include catch
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include <string>
#include <cstdlib>

#include "diseaseModel/CModel.h"
#include "plugins/CPlugin.h"
#include "utilities/CSimulation.h"
#include "network/CNetwork.h"
#include "traits/CTrait.h"
#include "utilities/CCommunicate.h"
#include "utilities/CRandom.h"
#include "utilities/CLogger.h"
#include "db/CSchema.h"
#include "db/CConnection.h"
#include "utilities/CSimConfig.h"
#include "utilities/CStatus.h"
#include "actions/CActionQueue.h"
#include "actions/CChanges.h"
#include "intervention/CInitialization.h"
#include "intervention/CIntervention.h"
#include "variables/CVariableList.h"
#include "sets/CSetReference.h"
#include "sets/CSetList.h"
#include "math/CSizeOf.h"
#include "utilities/CDirEntry.h"
#include "math/CObservable.h"

/**
 * Tries to find the test file in the srcdir environment variable.
 *
 * @param fileName the filename relative to this tests srcdir
 *        environment variable.
 *
 * If the filename cannot be found, the test from which this function
 * is called will fail.
 *
 * @return the full path to the test file
 */
std::string getAbsolutePath(const std::string& relativePath)
{
  char* srcDir = getenv("srcdir");

  if (srcDir != nullptr)
    {
      std::string AbsolutePath = relativePath;

      if (CDirEntry::makePathAbsolute(AbsolutePath, srcDir))
        return AbsolutePath;
    }

  // Default is to do nothing
  return relativePath;
}

void clearTest()
{
  CPlugin::clear();
  CModel::clear();
  CInitialization::clear();
  CIntervention::clear();
  CActionDefinition::clear();
  CObservable::clear();
  CSizeOf::Unique.clear();
  CSetContent::Unique.clear();
  CSetList::INSTANCE.reset();
  CComputable::Instances.reset();
  CNetwork::clear();
  CConnection::clear();
  CActionQueue::clear();
  CSimConfig::clear();
}

int main(int argc, char * argv[])
{
  int EXIT = EXIT_SUCCESS;

  CLogger::init();
  CCommunicate::init(argc, argv);

  // Random seeding
  CRandom::init(std::numeric_limits< size_t >::max());

  EXIT = Catch::Session().run(argc, argv);

  clearTest();

  CCommunicate::finalize();
  CLogger::finalize();

  return EXIT;
}
