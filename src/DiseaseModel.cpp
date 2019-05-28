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
