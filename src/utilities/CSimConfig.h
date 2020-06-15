// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
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

#include "utilities/CLogger.h"

struct json_t;

class CSimConfig
{
public:
  struct db_connection
  {
    std::string name;
    std::string host;
    std::string user;
    std::string password;
    size_t maxRecords;
    size_t connectionTimeout;
    size_t connectionRetries;
    size_t connectionMaxDelay;
  };

private:
  bool valid;

  std::string mRunParameters;

  // required
  std::string mModelScenario;
  std::string mDiseaseModel;
  std::string mContactNetwork;
  std::string mInitialization;
  std::string mTraits;
  std::vector< std::string > mPersonTraitDB;
  int mStartTick;
  int mEndTick;

  // optional
  std::string mOutput;
  std::string mSummaryOutput;
  std::string mStatus;
  std::string mIntervention;
  std::string mPropensityPlugin;
  size_t mSeed;
  size_t mReplicate;
  size_t mPartitionEdgeLimit;
  CLogger::LogLevel mLogLevel;
  db_connection mDBConnection;

private:
  static CSimConfig * INSTANCE;

  CSimConfig() = delete;
  CSimConfig(const std::string & configFile);
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
  static const std::string & getSummaryOutput();
  static const std::string & getStatus();
  static const std::string & getIntervention();
  static const std::string & getPropensityPlugin();
  static const size_t & getSeed();
  static const size_t & getReplicate();
  static const size_t & getPartitionEdgeLimit();
  static CLogger::LogLevel getLogLevel();
  static const db_connection & getDBConnection();

  static json_t * loadJson(const std::string & jsonFile, int flags);
  static json_t * loadJsonPreamble(const std::string & jsonFile, int flags);
};

#endif
