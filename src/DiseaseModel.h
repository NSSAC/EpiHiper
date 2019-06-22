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

#ifndef DISEASEMODEL_H
#define DISEASEMODEL_H

#include <string>
#include "Types.h"

struct Transition {
  health_t nextState;
  int delay;
};

class DiseaseModel {
private:
  health_t *states; // array of health states
  health_t initialState;
  infection_t *infections; // array of infections
  transition_t *transitions; // array of transitions
  bool isValid;
public:
  DiseaseModel(std::string modelJson);
  ~DiseaseModel();
  void validate();
  Transition getTransition(health_t currentState);
  health_t computeInfectedState(health_t currentState);
};

#endif
