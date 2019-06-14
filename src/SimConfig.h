#ifndef SIM_CONFIG_H
#define SIM_CONFIG_H

#include <string>

class SimConfig {
private:
  bool valid;

  // required
  std::string diseaseModelFile;
  std::string networkFile;
  std::string initializationFile;
  int startTick;
  int endTick;

  // optional
  std::string outputFile;
  std::string logFile;
  std::string interventionFile;

public:
  SimConfig(const std::string& configFile);
  ~SimConfig();

  // minimal set of parameters are specified and files are accessible
  void validate();

  inline bool isValid() const { return valid; }
  inline int getStartTick() const { return startTick; }
  inline int getEndTick() const { return endTick; }
  inline const std::string& getNetworkFile() const { return networkFile; }
  inline const std::string& getOutputFile() const { return outputFile; }

};

#endif
