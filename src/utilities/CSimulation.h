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

#ifndef SIMULATION_H
#define SIMULATION_H

#include "utilities/CCommunicate.h"

class CSimulation {
private:
  bool valid;
  int startTick;
  int endTick;

public:
  CSimulation();
  ~CSimulation();
  bool validate();
  bool run();
};

#endif