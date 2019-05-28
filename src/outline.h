class SimConfig {
private:
  bool valid;
  // required parameters
  std::string diseaseModelFile;
  std::string networkFile;
  std::string interventionFile;
  int startTick;
  int endTick;
  int numReplicates; // number of replicates to run

  // optional
  std::string outputFile;
  std::string logFile;
  std::string dbServiceUrl;
  std::string rngFile; // random number generating seeds

public:
  SimConfig(std::string configFile);
  ~SimConfig();

  // minimal set of parameters are specified and files are accessible
  validate();

  inline bool isValid() { return valid; }
};

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

class Node {
private:
  pid_t id;
  health_t currentState;
  bool local;
public:
  Node(pid_t id);
  ~Node();
  pid_t getId();
  health_t getCurrentState();
  bool isLocal();
  health_t computeInfection(DiseaseModel *disease, Network *network,
			    Population *pop);
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

class Action {
private:
  bool conditionsTrue;
  std::vector<ActionCondition> conditions;
public:
  Action();
  ~Action();
  checkConditions();
  conditionsTrue();
};

class ActionQueue {
private:
  std::vector<Action> actions;
public:
  ActionQueue();
  ~ActionQueue();
  void schedule(Action action);
  // get actions scheduled for tick
  std::vector<Action> getActions(int tick);
  // removed actions scheduled for tick
  void clearActions(int tick);
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
