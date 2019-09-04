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

#ifndef SIM_CONFIG_H
#define SIM_CONFIG_H

#include <vector>
#include <string>

struct json_t;

class CSimConfig {
public:
  struct db_connection
  {
    std::string name;
    std::string host;
    std::string user;
    std::string password;
  };

private:
  bool valid;

  std::string runParameters;

  // required
  std::string modelScenario;
  std::string diseaseModel;
  std::string contactNetwork;
  std::string initialization;
  std::string traits;
  std::vector< std::string > personTraitDB;
  int startTick;
  int endTick;

  // optional
  std::string output;
  std::string logFile;
  std::string intervention;
  size_t seed;
  size_t replicate;
  db_connection dbConnection;

private:
  static CSimConfig * INSTANCE;

  CSimConfig() = delete;
  CSimConfig(const std::string& configFile);
  ~CSimConfig();

  bool loadScenario();

public:
  static void load(const std::string & configFile);
  static void release();

  static bool isValid();
  static int getStartTick();
  static int getEndTick();
  static const std::string & getDiseaseModel();
  static const std::string & getContactNetwork();
  static const std::string & getInitialization();
  static const std::string & getTraits();
  static const std::vector< std::string > & getPersonTraitDB();
  static const std::string & getOutput();
  static const std::string & getIntervention();
  static const size_t & getSeed();
  static const size_t & getReplicate();
  static const db_connection & getDBConnection();

  static json_t * loadJson(const std::string & jsonFile, int flags);


};

#endif
