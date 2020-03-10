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
#include <getopt.h>

#include "diseaseModel/CModel.h"
#include "EpiHiperConfig.h"
#include "Simulation.h"
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

std::string Config = std::string();
int seed = -1;
std::string dbconn = std::string();

void printUsage()
{
  std::cout << "\nUsage:\n\n"
               "EpiHiper --config <configFilename> --seed <integerValue>"
               " --dbconn <jsonString>\n"
               "--config: required string specifying EpiHiper configuration file\n"
               "--seed: optional integer (>=0) as seed for random number generation\n"
               "--dbconn <jsonString>: JSON with connection info to personTraitDB\n";
  //exit(EXIT_FAILURE);
}

bool parseArgs(int argc, char * argv[])
{
  const char * const short_opts = "c:s:d:";
  const option long_opts[] = {
    {"config", required_argument, nullptr, 'c'},
    {nullptr, no_argument, nullptr, 0}};
  while (true)
    {
      const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);
      if (-1 == opt)
        break;
      switch (opt)
        {
        case 'c':
          Config = std::string(optarg);
          break;
        case '?':
        default:
          return false;
        }
    }
  return true;
}

int main(int argc, char * argv[])
{
  int EXIT = EXIT_SUCCESS;
  CLogger::init();
  CCommunicate::init(argc, argv);

  if (CCommunicate::MPIRank == 0)
    {
      std::cout << "EpiHiper Version "
                << EpiHiper_VERSION_MAJOR << "." << EpiHiper_VERSION_MINOR << "." << EpiHiper_VERSION_PATCH
                << " (build: " << __DATE__ << ", commit: " << GIT_COMMIT << ")" << std::endl;
    }

  if (argc < 3 || !parseArgs(argc, argv))
    {
      if (CCommunicate::MPIRank == 0)
        {
          printUsage();
        }
        
      CCommunicate::abort(CCommunicate::ErrorCode::InvalidArguments);
      CCommunicate::finalize();

      exit(EXIT_FAILURE);
    }

#ifdef DEBUG_WAIT
  int debugwait = (CCommunicate::MPIRank == 0);

  printf("Rank: %d, PID: %d\n", CCommunicate::MPIRank, getpid());

  while (debugwait)
    sleep(1);
#endif

  CSimConfig::load(Config);

  if (!CSimConfig::isValid()
      || CLogger::hasErrors())
    {
      CStatus::load("EpiHiper");
      goto failed;
    }

  CStatus::load("EpiHiper");
  CActionQueue::init();
  CRandom::init();
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
    
  CModel::load(CSimConfig::getDiseaseModel());

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

  CNetwork::INSTANCE->load();

  if (CLogger::hasErrors())
    {
      goto failed;
    }

  CCommunicate::memUsage(-2);

  {
    Simulation sim(seed, dbconn);

    if (!sim.validate())
      goto failed;

    if (!sim.run())
      goto failed;
  }

  CStatus::finalize("EpiHiper");
  goto success;

failed:
  EXIT = EXIT_FAILURE;
  CStatus::update("EpiHiper", "failed");
  CCommunicate::abort((CCommunicate::ErrorCode) MPI_ERR_UNKNOWN);

success:
  CModel::release();
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
