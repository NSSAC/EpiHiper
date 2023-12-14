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
      CLogger::finalize();

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

  CAnalyzer::LoadModel();

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
  CLogger::finalize();

  exit(EXIT);
}
