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

#include <getopt.h>
#include <iostream>

#include "EpiHiperConfig.h"
#include "utilities/CArgs.h"
#include "utilities/CDirEntry.h"

// static 
std::string CArgs::Config;

//static 
std::string CArgs::Name;

//static 
std::string CArgs::Path;

bool CArgs::parseArgs(int argc, char * argv[])
{
  Config.clear();
  Path.clear();
  Name.clear();

  if (argc == 0)
    return false; 

  Path = argv[0];
  Name = CDirEntry::fileName(Path);

  const char * const short_opts = "c";

  const option long_opts[] =
    {
      {"config", required_argument, nullptr, 'c'},
      {nullptr, no_argument, nullptr, 0}
    };

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

  return !Config.empty();
}

void CArgs::printUsage()
{
  std::cout << std::endl
            << "Usage:" << std::endl
            << "  " << Name << " --config <configFilename>" << std::endl;
}

void CArgs::printWhoAmI()
{
  std::cout << Name << " Version "
            << EpiHiper_VERSION_MAJOR << "." << EpiHiper_VERSION_MINOR << "." << EpiHiper_VERSION_PATCH
            << " (build: " << __DATE__ << ", commit: " << GIT_COMMIT << ")" << std::endl;
}

const std::string & CArgs::getConfig()
{
  return Config;
}

// static 
const std::string & CArgs::getName()
{
  return Name;
}

// static 
const std::string & CArgs::getPath()
{
  return Path;
}

