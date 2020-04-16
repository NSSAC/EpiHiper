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

#ifndef SRC_CARGS_H_
#define SRC_CARGS_H_

#include <string>

class CArgs
{
public:
  static bool parseArgs(int argc, char * argv[]);

  static void printUsage();

  static void printWhoAmI();

  static const std::string & getConfig();

  static const std::string & getName();

  static const std::string & getPath();

private:
  static std::string Config;
  static std::string Name;
  static std::string Path;
};

#endif // SRC_CARGS_H_
