#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "SimConfig.h"
#include "Misc.h"

// constructor: parse JSON
SimConfig::SimConfig(const std::string& configFile) {
  valid = false;
  if (configFile.empty()) {
    std::cerr << "Simulation configuration file is not specified" << std::endl;
  }
  else {
    std::ifstream fsConfig(configFile.data(), std::ios_base::binary);
    if (fsConfig.good()) {
      std::stringstream buffer;
      buffer << fsConfig.rdbuf();
      std::string s = trim(buffer.str(), "{} \n\t\n\r\f");

      // parse parameter:value pairs
      std::vector<std::string> pairs = split(s, ',');
      int n = pairs.size();
      for (int i = 0; i < n; ++i) {
	std::vector<std::string> keyValue = split(pairs[i],':');
	std::string key = trim(keyValue[0]," \n\t\v\r\f\"");
	std::string value = trim(keyValue[1]," \n\t\v\r\f\"");

	if (key == "contactNetwork") {
	  networkFile = value;
	}
	if (key == "outputFile") {
	  outputFile = value;
	}
	if (key == "startTick") {
	  startTick = stoi(value);
	}
	if (key == "endTick") {
	  endTick = stoi(value);
	}
	// TODO: parse other keys
      }
    }
    else {
      std::cerr << "Fail to open file " << configFile << std::endl;
    }
  }
}

SimConfig::~SimConfig() {
}

void SimConfig::validate() {
  valid = true;
  std::ifstream f(networkFile.data());
  if (! f.good()) valid = false;
}
