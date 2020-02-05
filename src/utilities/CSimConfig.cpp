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

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <jansson.h>
#include <limits>
#include <cstring>

#include "utilities/CSimConfig.h"
#include "utilities/CDirEntry.h"
#include "utilities/CLogger.h"

// static
void CSimConfig::load(const std::string & configFile)
{
  if (CSimConfig::INSTANCE == NULL)
    {
      CSimConfig::INSTANCE = new CSimConfig(configFile);
    }
}

// static
void CSimConfig::release()
{
  if (CSimConfig::INSTANCE != NULL)
    {
      delete CSimConfig::INSTANCE;
      CSimConfig::INSTANCE = NULL;
    }
}

// static
bool CSimConfig::isValid()
{
  return CSimConfig::INSTANCE->valid;
}

// static
int CSimConfig::getStartTick()
{
  return CSimConfig::INSTANCE->mStartTick;
}

// static
int CSimConfig::getEndTick()
{
  return CSimConfig::INSTANCE->mEndTick;
}

// static
const std::string & CSimConfig::getDiseaseModel()
{
  return CSimConfig::INSTANCE->mDiseaseModel;
}

// static
const std::string & CSimConfig::getContactNetwork()
{
  return CSimConfig::INSTANCE->mContactNetwork;
}

// static
const std::string & CSimConfig::getInitialization()
{
  return CSimConfig::INSTANCE->mInitialization;
}

// static
const std::string & CSimConfig::getTraits()
{
  return CSimConfig::INSTANCE->mTraits;
}

// static
const std::vector< std::string > & CSimConfig::getPersonTraitDB()
{
  return CSimConfig::INSTANCE->mPersonTraitDB;
}

// static
const std::string & CSimConfig::getOutput()
{
  return CSimConfig::INSTANCE->mOutput;
}

// static
const std::string & CSimConfig::getSummaryOutput()
{
  return CSimConfig::INSTANCE->mSummaryOutput;
}

// static
const std::string & CSimConfig::getStatus()
{
  return CSimConfig::INSTANCE->mStatus;
}

// static
const std::string & CSimConfig::getIntervention()
{
  return CSimConfig::INSTANCE->mIntervention;
}

// static
const size_t & CSimConfig::getSeed()
{
  return CSimConfig::INSTANCE->mSeed;
}

// static
const size_t & CSimConfig::getReplicate()
{
  return CSimConfig::INSTANCE->mReplicate;
}

// static
CSimConfig::LogLevel CSimConfig::getLogLevel()
{
  if (CSimConfig::INSTANCE != NULL)
    return CSimConfig::INSTANCE->mLogLevel;

  return spdlog::level::warn;
}

// static
const CSimConfig::db_connection & CSimConfig::getDBConnection()
{
  return CSimConfig::INSTANCE->mDBConnection;
}

// constructor: parse JSON
CSimConfig::CSimConfig(const std::string & configFile)
  : valid(false)
  , mRunParameters(configFile)
  , mModelScenario()
  , mDiseaseModel()
  , mContactNetwork()
  , mInitialization()
  , mTraits()
  , mPersonTraitDB()
  , mStartTick(std::numeric_limits< int >::min())
  , mEndTick(std::numeric_limits< int >::max())
  , mOutput()
  , mSummaryOutput()
  , mStatus()
  , mIntervention()
  , mSeed(-1)
  , mReplicate(-1)
  , mDBConnection()
{
  if (mRunParameters.empty())
    {
      spdlog::error("Simulation configuration file is not specified");
      return;
    }

  CDirEntry::makePathAbsolute(mRunParameters, CDirEntry::getPWD());

  json_t * pRoot = loadJson(configFile, JSON_DECODE_INT_AS_REAL);

  if (pRoot == NULL)
    {
      return;
    }

  valid = true;

  json_t * pValue = json_object_get(pRoot, "modelScenario");

  if (json_is_string(pValue))
    {
      mModelScenario = CDirEntry::resolve(json_string_value(pValue), mRunParameters);
    }

  valid &= !mModelScenario.empty();

  std::string DefaultDir;

  if (CDirEntry::exist("/output")
      && CDirEntry::isWritable("/output"))
    DefaultDir = "/output";
  else
    DefaultDir = ".";

  pValue = json_object_get(pRoot, "output");

  if (json_is_string(pValue))
    mOutput = json_string_value(pValue);
  else
    mOutput = "output.csv";

  mOutput = CDirEntry::resolve(mOutput, mRunParameters, DefaultDir);

  if (!CDirEntry::exist(CDirEntry::dirName(mOutput)))
    CDirEntry::createDir(CDirEntry::dirName(mOutput));

  pValue = json_object_get(pRoot, "summaryOutput");

  if (json_is_string(pValue))
    mSummaryOutput = json_string_value(pValue);
  else
    mSummaryOutput = "summaryOutput.csv";

  mSummaryOutput = CDirEntry::resolve(mSummaryOutput, mRunParameters, DefaultDir);

  if (!CDirEntry::exist(CDirEntry::dirName(mSummaryOutput)))
    CDirEntry::createDir(CDirEntry::dirName(mSummaryOutput));

  if (CDirEntry::exist("/job")
      && CDirEntry::isWritable("/job"))
    DefaultDir = "/job";
  else
    DefaultDir = ".";

  pValue = json_object_get(pRoot, "status");

  if (json_is_string(pValue))
    mStatus = json_string_value(pValue);
  else
    mStatus = "sciduct.status.json";

  mStatus = CDirEntry::resolve(mStatus, mRunParameters, DefaultDir);

  if (!CDirEntry::exist(CDirEntry::dirName(mStatus)))
    CDirEntry::createDir(CDirEntry::dirName(mStatus));

  pValue = json_object_get(pRoot, "startTick");

  if (json_is_real(pValue))
    {
      mStartTick = json_real_value(pValue);
    }

  valid &= mStartTick != std::numeric_limits< int >::min();

  pValue = json_object_get(pRoot, "endTick");

  if (json_is_real(pValue))
    {
      mEndTick = json_real_value(pValue);
    }

  valid &= mEndTick != std::numeric_limits< int >::max()
           && mStartTick < mEndTick;

  pValue = json_object_get(pRoot, "seed");

  if (json_is_real(pValue))
    {
      mSeed = json_real_value(pValue);
    }

  pValue = json_object_get(pRoot, "replicate");

  if (json_is_real(pValue))
    {
      mReplicate = json_real_value(pValue);
    }

  pValue = json_object_get(pRoot, "logLevel");

  if (json_is_string(pValue))
    {
      const char * LogLevel = json_string_value(pValue);
      mLogLevel = spdlog::level::from_str(LogLevel);

      // If the LogLevel is not found off is returned but we default to warn
      if (mLogLevel == spdlog::level::off
          && strcmp(LogLevel, "off") != 0)
        {
          mLogLevel = spdlog::level::warn;
        }
    }
  else
    {
      mLogLevel = LogLevel::warn;
    }

  spdlog::set_level(mLogLevel);

  pValue = json_object_get(pRoot, "dbName");

  if (json_is_string(pValue))
    {
      mDBConnection.name = json_string_value(pValue);
    }
  else
    {
      mDBConnection.name = "epihiper_db";
    }

  pValue = json_object_get(pRoot, "dbHost");

  if (json_is_string(pValue))
    {
      mDBConnection.host = json_string_value(pValue);
    }
  else
    {
      mDBConnection.host = "localhost:5432";
    }

  pValue = json_object_get(pRoot, "dbUser");

  if (json_is_string(pValue))
    {
      mDBConnection.user = json_string_value(pValue);
    }
  else
    {
      mDBConnection.user = "epihiper";
    }

  pValue = json_object_get(pRoot, "dbPassword");

  if (json_is_string(pValue))
    {
      mDBConnection.password = json_string_value(pValue);
    }
  else
    {
      mDBConnection.password.clear();
    }

  pValue = json_object_get(pRoot, "dbMaxRecords");

  if (json_is_real(pValue))
    {
      mDBConnection.maxRecords = json_real_value(pValue);
    }
  else
    {
      mDBConnection.maxRecords = 100000;
    }

  json_decref(pRoot);

  valid &= loadScenario();
}

CSimConfig::~CSimConfig()
{
}

bool CSimConfig::loadScenario()
{
  json_t * pRoot = loadJson(mModelScenario, JSON_DECODE_INT_AS_REAL);

  if (pRoot == NULL)
    return false;

  json_t * pValue = json_object_get(pRoot, "contactNetwork");

  if (json_is_string(pValue))
    {
      mContactNetwork = CDirEntry::resolve(json_string_value(pValue), mModelScenario);
    }

  pValue = json_object_get(pRoot, "diseaseModel");

  if (json_is_string(pValue))
    {
      mDiseaseModel = CDirEntry::resolve(json_string_value(pValue), mModelScenario);
    }

  pValue = json_object_get(pRoot, "initialization");

  if (json_is_string(pValue))
    {
      mInitialization = CDirEntry::resolve(json_string_value(pValue), mModelScenario);
    }

  pValue = json_object_get(pRoot, "intervention");

  if (json_is_string(pValue))
    {
      mIntervention = CDirEntry::resolve(json_string_value(pValue), mModelScenario);
    }

  pValue = json_object_get(pRoot, "traits");

  if (json_is_string(pValue))
    {
      mTraits = CDirEntry::resolve(json_string_value(pValue), mModelScenario);
    }

  pValue = json_object_get(pRoot, "personTraitDB");

  if (json_is_array(pValue))
    for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i)
      {
        json_t * pDB = json_array_get(pValue, i);

        if (json_is_string(pDB))
          {
            mPersonTraitDB.push_back(CDirEntry::resolve(json_string_value(pDB), mModelScenario));
          }
      }

  json_decref(pRoot);

  return !mContactNetwork.empty()
         && !mDiseaseModel.empty()
         && !mIntervention.empty()
         && !mInitialization.empty();
}

// static
json_t * CSimConfig::loadJson(const std::string & jsonFile, int flags)
{
  json_t * pRoot = NULL;

  if (jsonFile.empty())
    {
      spdlog::error("JSON file is not specified");
      return pRoot;
    }

  std::ifstream is(jsonFile.c_str());

  if (is.fail())
    {
      spdlog::error("JSON file: '" + jsonFile + "' cannot be opened.");
      return pRoot;
    }

  // Read only first line:
  std::string Line;
  std::getline(is, Line);

  json_error_t error;

  pRoot = json_loads(Line.c_str(), flags, &error);

  if (pRoot != NULL)
    return pRoot;

  // get length of file:
  is.seekg(0, is.end);
  size_t length = is.tellg();
  is.seekg(0, is.beg);

  char * buffer = new char[length + 1];
  is.read(buffer, length);
  buffer[length] = 0;

  pRoot = json_loads(buffer, flags, &error);
  delete[] buffer;

  if (pRoot == NULL)
    {
      std::ostringstream msg;
      msg << "JSON file: '" << jsonFile << "' error on line " << error.line << ": " << error.text << std::endl;

      spdlog::error(msg.str());
    }

  return pRoot;
}
