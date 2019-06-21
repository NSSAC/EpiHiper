#ifndef SIM_CONFIG_H
#define SIM_CONFIG_H

#include <string>

struct json_t;

class SimConfig {
private:
  bool valid;

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

public:
  SimConfig(const std::string& configFile);
  ~SimConfig();

  // minimal set of parameters are specified and files are accessible
  void validate();

  inline bool isValid() const { return valid; }
  inline int getStartTick() const { return startTick; }
  inline int getEndTick() const { return endTick; }
  inline const std::string& getNetworkFile() const { return contactNetwork; }
  inline const std::string& getOutputFile() const { return output; }

private:
  json_t * loadJson(const std::string & jsonFile) const;


};

#endif
