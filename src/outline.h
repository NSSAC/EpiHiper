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

class PersonTraitDB {
private:
  std::string url; // ip:port
  bool connected;

public:
  PersonTraitDB();
  virtual ~PersonTraitDB();

  // establish connection to DB service
  connect();
  // terminate connection to DB service
  disconnect();

  // send query and get results
  std::string query(std::string statement);

  // need a few parsing methods, e.g.
  // pid_t parse(std::string queryResults);

  // write data to DB; data in JSON, with metadata (query statement)
  bool write(std::string data);
};

class Simulation {
private:
  int startTick;
  int endTick;
  std::vector<Node> population; // the set of all nodes
  std::vector<Intervention> initialization;
  std::vector<Intervention> interventions;
  DiseaseModel *disease;
  // partition ???
  Network *network;
  ActionQueue *actionQueue;
  ComputingEnvironment *compEnv; // MPI context
  PersonTraitDB *db;
  Output *output;
  Log *log;

public:
  Simulation(SimConfig cfg);
  ~Simulation();
  void readDiseaseModel();
  void partition();
  void readNetwork();
  void initializeNodes();
  void readInterventions();

  void validate();
  void run();  
};

class Network {
private:
  // TBD: data structure to store node ids and incoming edges
public:
  std::vector<pid_t> getNeighbors(pid_t id);
  bool isLocal(pid_t id);
};

class DiseaseModel {
private:
  // diagram
public:
  DiseaseModel(std::string modelJson);
  ~DieaseModel();
  void validate();
  Transition getTransition(health_t currentState);
};

class Intervention {
private:
  // filters, actions, etc.
public:
  Intervention(std::string interventionJson);
  ~Intervention();
  std::vector<Node> computeCandidates(Network *network,
				      std::vector<Node> population);
};

class ComputingEnvironment {
private:
  // PE rank, master PE for global variables, etc.
public:
  void exchange(changes);
};

struct Transition {
  health_t nextState;
  int delay;
};
