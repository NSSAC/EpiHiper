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

#include "DiseaseModel.h"

// constructor: parse JSON
DiseaseModel::DiseaseModel(std::string modelJson) {
  states = NULL;
  infections = NULL;
  transitions = NULL;
}

DiseaseModel::~DiseaseModel() {
}

void DiseaseModel::validate() {
  isValid = true;
  // check transitions: sum of prob from any entryState == 1
}

Transition DiseaseModel::getTransition(health_t currentState) {
  Transition t;
  // compute successor state t.nextState
  // compute delay t.delay
  return t;
}
