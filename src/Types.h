// BEGIN: Copyright 
// Copyright (C) 2019 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#ifndef EPIHIPER_TYPES_H
#define EPIHIPER_TYPES_H

#include <cstdint>
#include <string>

typedef uint64_t personid_t;
typedef uint64_t netsize_t;
struct health_t {
  std::string id;
  std::string label;
  float susceptibility;
  float infectivity;
};
struct infection_t {
  std::string id;
  std::string label;
  health_t entryState;
  health_t exitState;
  health_t contactState;
  float transmissibility;
};
struct transition_t {
  std::string id;
  std::string label;
  health_t entryState;
  health_t exitState;
  float prob;
  // Distribution dwellTime; // TODO: class Distribution
};

#endif
