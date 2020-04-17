// BEGIN: Copyright 
// Copyright (C) 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include "utilities/CArgs.h"

#include "diseaseModel/CAnalyzer.h"
#include "utilities/CCommunicate.h"
#include "utilities/CRandom.h"
#include "utilities/CLogger.h"
#include "utilities/CStatus.h"

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

  CAnalyzer::Init();

  CAnalyzer::Load(CArgs::getConfig());

  if (CLogger::hasErrors())
    {
      CStatus::load("analyzer", CArgs::getName(), CAnalyzer::StatusFile());
      goto failed;
    }

  CStatus::load("analyzer", CArgs::getName(), CAnalyzer::StatusFile());
  CRandom::init(CAnalyzer::Seed());

  if (CLogger::hasErrors())
    {
      goto failed;
    }

  CAnalyzer::Run();

  if (CLogger::hasErrors())
    {
      goto failed;
    }
    

  CStatus::success();
  goto success;

failed:
  EXIT = EXIT_FAILURE;
  CStatus::update("failed");
  CCommunicate::abort((CCommunicate::ErrorCode) MPI_ERR_UNKNOWN);

success:
  CAnalyzer::Release();
  CCommunicate::finalize();
  CLogger::release();

  exit(EXIT);
}
