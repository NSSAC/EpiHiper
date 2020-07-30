// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include <iostream>
#include <cstdlib>

#include "utilities/CArgs.h"

#include "diseaseModel/CModel.h"
#include "diseaseModel/CTransmissionPropensity.h"
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

// Uncomment the following line if you want to attache a debugger
// #define DEBUG_WAIT 1

#ifdef DEBUG_WAIT
#  include <sys/types.h>
#  include <unistd.h>
#endif // DEBUG_WAIT

int main(int argc, char * argv[])
{
  int EXIT = EXIT_SUCCESS;
  CLogger::init();
  CCommunicate::init(argc, argv);
  bool ArgumentsValid = CArgs::parseArgs(argc, argv);

  if (CCommunicate::MPIRank == 0)
    CArgs::printWhoAmI();

  if (!ArgumentsValid)
    {
      if (CCommunicate::MPIRank == 0)
        CArgs::printUsage();
        
      CCommunicate::abort(CCommunicate::ErrorCode::InvalidArguments);
      CCommunicate::finalize();
      CLogger::release();

      exit(EXIT_FAILURE);
    }

#ifdef DEBUG_WAIT
  int debugwait = (CCommunicate::MPIRank == 0);

  printf("Rank: %d, PID: %d\n", CCommunicate::MPIRank, getpid());

  while (debugwait)
    sleep(1);
#endif

  CSimConfig::load(CArgs::getConfig());

  if (!CSimConfig::isValid()
      || CLogger::hasErrors())
    {
      CStatus::load("epihiper", CArgs::getName(), CSimConfig::getStatus());
      goto failed;
    }

  CStatus::load("epihiper", CArgs::getName(), CSimConfig::getStatus());
  CTransmissionPropensity::Init();

  if (CLogger::hasErrors())
    {
      goto failed;
    }

  CActionQueue::init();
  CRandom::init(CSimConfig::getSeed());
  CTrait::init();

  if (CLogger::hasErrors())
    {
      goto failed;
    }

  CNetwork::init();

  if (CLogger::hasErrors())
    {
      goto failed;
    }

  CTrait::load(CSimConfig::getTraits());

  if (CLogger::hasErrors())
    {
      goto failed;
    }

  CModel::Load(CSimConfig::getDiseaseModel());

  if (CLogger::hasErrors())
    {
      goto failed;
    }

  CSchema::load(CSimConfig::getPersonTraitDB());

  if (CLogger::hasErrors())
    {
      goto failed;
    }

  CInitialization::load(CSimConfig::getInitialization());

  if (CLogger::hasErrors())
    {
      goto failed;
    }

  CIntervention::load(CSimConfig::getIntervention());

  if (CLogger::hasErrors())
    {
      goto failed;
    }

  if (!CSetReference::resolve())
    {
      goto failed;
    }

  if (CCommunicate::allocateRMA() != (int) CCommunicate::ErrorCode::Success)
    {
      goto failed;
    }

  CConnection::init();

  if (CLogger::hasErrors())
    {
      goto failed;
    }

  CVariableList::INSTANCE.resetAll(true);

  CNetwork::Context.Master().load();

  if (CLogger::hasErrors())
    {
      goto failed;
    }

  CChanges::determineNodesRequested();
  CCommunicate::memUsage(-2);

  {
    CSimulation sim;

    if (!sim.validate())
      goto failed;

    if (!sim.run())
      goto failed;
  }

  CStatus::success();
  goto success;

failed:
  EXIT = EXIT_FAILURE;
  CStatus::update("failed");
  CCommunicate::abort((CCommunicate::ErrorCode) MPI_ERR_UNKNOWN);

success:
  CTransmissionPropensity::Release();
  CModel::Release();
  CInitialization::release();
  CIntervention::release();
  CNetwork::release();
  CConnection::release();
  CActionQueue::release();
  CSimConfig::release();
  CCommunicate::finalize();
  CLogger::release();

  exit(EXIT);
}
