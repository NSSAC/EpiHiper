#ifndef SIMULATION_H
#define SIMULATION_H

#include "SimConfig.h"
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
  Simulation(SimConfig cfg, int seed, std::string dbconn);
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
};

struct DummyTransition {
  int tick;
  personid_t id;
  std::string exitState;
  std::string idContact;
};
  
#endif
