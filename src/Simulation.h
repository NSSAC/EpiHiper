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

#ifndef SIMULATION_H
#define SIMULATION_H

#include "utilities/Communicate.h"

#include "Types.h"

class Simulation {
private:
  bool valid;
  int randomSeed;
  int startTick;
  int endTick;
  /*
  std::vector<Node> population; // the set of all nodes
  std::vector<Intervention> initialization;
  std::vector<Intervention> interventions;
  DiseaseModel *disease;
  Network *network;
  ActionQueue *actionQueue;
  ComputingEnvironment *compEnv; // MPI context
  PersonTraitDB *db;
  Output *output;
  Log *log;
  */

  std::string networkFile;
  std::string outputFile;

public:
  Simulation(int seed, std::string dbconn);
  ~Simulation();
  void readDiseaseModel();
  void partition();
  void readNetwork();
  void initializeNodes();
  void readInterventions();
  void validate();
  void run();
  void dummyRun();
  inline bool isValid() { return valid; }
  Communicate::ErrorCode writedata();
};

struct DummyTransition {
  int tick;
  personid_t id;
  std::string exitState;
  std::string idContact;
};
  
#endif
