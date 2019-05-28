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
