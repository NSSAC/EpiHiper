#ifndef SIM_CONFIG_H
#define SIM_CONFIG_H

#include <string>

struct json_t;

class SimConfig {
private:
  bool valid;

  std::string runParameters;

  // required
  std::string diseaseModel;
  std::string contactNetwork;
  std::string initialization;
  std::string traits;
  std::string personTraitDB;
  int startTick;
  int endTick;

  // optional
  std::string output;
  std::string logFile;
  std::string intervention;

private:
  static SimConfig * CONFIG;

  SimConfig() = delete;
  SimConfig(const std::string& configFile);
  ~SimConfig();

public:
  static void init(const std::string & configFile);
  static void release();

  static bool isValid();
  static int getStartTick();
  static int getEndTick();
  static const std::string& getDiseaseModel();
  static const std::string& getContactNetwork();
  static const std::string& getInitialization();
  static const std::string& getTraits();
  static const std::string& getPersonTraitDB();
  static const std::string& getOutput();
  static const std::string& getIntervention();
  static json_t * loadJson(const std::string & jsonFie);


};

#endif
