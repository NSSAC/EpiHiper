#ifndef SIMULATION_H
#define SIMULATION_H

class Simulation {
private:
  bool valid;
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
  inline bool isValid() { return valid; }
};

#endif
