// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2020 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
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
            << " (build: " << __DATE__ << ", commit: " << GIT_COMMIT
            << ", MPI: "
#ifdef USE_MPI
            << "ON"
#else
            << "OFF"
#endif // USE_MPI
            << ", OpenMP: "
#ifdef USE_OMP
            << "ON"
#else
            << "OFF"
#endif // USE_OMP
            << ", LID: "
#ifdef USE_LOCATION_ID
            << "ON"
#else
            << "OFF"
#endif // USE_OMP
            << ")" << std::endl;
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

