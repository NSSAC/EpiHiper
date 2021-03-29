// BEGIN: Copyright 
// Copyright (C) 2019 - 2021 Rector and Visitors of the University of Virginia 
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
const std::string & CSimConfig::getPropensityPlugin()
{
  return CSimConfig::INSTANCE->mPropensityPlugin;
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
const size_t & CSimConfig::getPartitionEdgeLimit()
{
  return CSimConfig::INSTANCE->mPartitionEdgeLimit;
}

// static
CLogger::LogLevel CSimConfig::getLogLevel()
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

// static 
const CSimConfig::dump_active_network & CSimConfig::getDumpActiveNetwork()
{
  return CSimConfig::INSTANCE->mDumpActiveNetwork;
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
  , mPropensityPlugin()
  , mSeed(-1)
  , mReplicate(-1)
  , mPartitionEdgeLimit(100000000)
  , mDBConnection()
{
  if (mRunParameters.empty())
    {
      CLogger::error("Simulation configuration file is not specified");
      return;
    }

  CDirEntry::makePathAbsolute(mRunParameters, CDirEntry::getPWD());

  json_t * pRoot = loadJson(configFile, JSON_DECODE_INT_AS_REAL);
/*
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "EpiHiper run parameters",
  "description": "A schema describing the run parameters for EpiHiper",
  "definitions": {},
  "type": "object",
  "required": [
    "epiHiperSchema",
    "modelScenario",
    "startTick",
    "endTick"
  ],
  "properties": {
    "epiHiperSchema": {
      "type": "string",
      "pattern": "^./runParametersSchema.json$"
    },
    "modelScenario": {
      "description": "Filename of the disease model in a json file.",
      "allOf": [
        {"$ref": "./typeRegistry.json#/definitions/localPath"},
        {"$ref": "./typeRegistry.json#/definitions/jsonFormat"}
      ]
    },
    "startTick": {
      "description": "The start tick for the simulation run",
      "type": "number",
      "minimum": 0,
      "multipleOf": 1.0
    },
    "endTick": {
      "description": "The start tick for the simulation run",
      "type": "number",
      "minimum": 0,
      "multipleOf": 1.0
    },
    "output": {
      "description": "Path + name of the output file",
      "$ref": "./typeRegistry.json#/definitions/localPath"
    },
    "summaryOutput": {
      "description": "Path + name of the summary output file",
      "$ref": "./typeRegistry.json#/definitions/localPath"
    },
    "status": {
      "description": "Path + name of the output SciDuct status file",
      "allOf": [
        {"$ref": "./typeRegistry.json#/definitions/localPath"},
        {"$ref": "./typeRegistry.json#/definitions/jsonFormat"}
      ]
    },
    "seed": {
      "description": "The seed for the random number generator",
      "$ref": "./typeRegistry.json#/definitions/nonNegativeInteger"
    },
    "replicate": {
      "description": "The number of the replicate created with the job",
      "$ref": "./typeRegistry.json#/definitions/nonNegativeInteger"
    },
    "partitionEdgeLimit": {
      "description": "The maximum number of network edges which are partitioned on the fly.",
      "$ref": "./typeRegistry.json#/definitions/nonNegativeInteger"
    },
    "logLevel": {
      "description": "The logging level (default warn)",
      "type": "string",
      "enum": [
        "trace",
        "debug",
        "info",
        "warn",
        "error",
        "critical",
        "off"
      ]
    },
    "dbName": {
      "description": "The name of the data base",
      "type": "string"
    },
    "dbHost": {
      "description": "The host of the database (including optional port)",
      "type": "string"
    },
    "dbUser": {
      "description": "The database user",
      "type": "string"
    },
    "dbPassword": {
      "description": "The password of the DB user",
      "type": "string"
    },
    "dbMaxRecords": {
      "description": "The maximal number of records returned by a single query (default: 100,000; 0: unlimited)",
      "$ref": "./typeRegistry.json#/definitions/nonNegativeInteger"
    },
    "dbConnectionTimeout": {
      "description": "The maximal number of second to wait for a connection (default: 2)",
      "type": "number",
      "multipleOf": 1,
      "minimum": 2
    },
    "dbConnectionRetries": {
      "description": "The maximal number of retries for a connection (default: 15)",
      "$ref": "./typeRegistry.json#/definitions/nonNegativeInteger"
    },
    "dbConnectionMaxDelay": {
      "description": "The maximal delay in milli seconds for attempting a connection (default: 500)",
      "$ref": "./typeRegistry.json#/definitions/nonNegativeInteger"
    },
    "dumpActiveNetwork": {
      "type": "object",
      "description": "If present causes regular dumps of the active network",
      "required": [
        "output",
        "threshhold"
      ],
      "properties": {
        "threshhold": {
          "description": "The threshhold for the weight for which active edges are dumped.",
          "type": "number",
          "minimum": 0
        },
        "output": {
          "description": "Path + name of the output file (default: /output/contactNetwork)",
          "$ref": "./typeRegistry.json#/definitions/localPath"
        },
        "startTick": {
          "description": "The start tick for the simulation run (default: startTick of simulation).",
          "type": "number",
          "minimum": 0,
          "multipleOf": 1.0
        },
        "endTick": {
          "description": "The start tick for the simulation run (default: endTick of simulation).",
          "type": "number",
          "minimum": 0,
          "multipleOf": 1.0
        },
        "tickIncrement": {
          "description": "The number of ticks between network dumps (default: 1).",
          "type": "number",
          "minimum": 0,
          "multipleOf": 1.0
        },
        "format": {
          "description": "The format of the network file (default: text).",
          "enum": [
            "text",
            "binary"
          ]
        }
      }
    }
  }
}
*/
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

  pValue = json_object_get(pRoot, "propensityPlugin");

  if (json_is_string(pValue))
    {
      mPropensityPlugin = CDirEntry::resolve(json_string_value(pValue), mRunParameters);
    }

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

  std::string DefauJobDirtDir;

  if (CDirEntry::exist("/job")
      && CDirEntry::isWritable("/job"))
    DefauJobDirtDir = "/job";
  else
    DefauJobDirtDir = ".";

  pValue = json_object_get(pRoot, "status");

  if (json_is_string(pValue))
    mStatus = json_string_value(pValue);
  else
    mStatus = "sciduct.status.json";

  mStatus = CDirEntry::resolve(mStatus, mRunParameters, DefauJobDirtDir);

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
           && mStartTick <= mEndTick;

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

  pValue = json_object_get(pRoot, "partitionEdgeLimit");

  if (json_is_real(pValue))
    {
      mPartitionEdgeLimit = json_real_value(pValue);
    }

  pValue = json_object_get(pRoot, "logLevel");

  if (json_is_string(pValue))
    {
      const char * LogLevel = json_string_value(pValue);
      mLogLevel = spdlog::level::from_str(LogLevel);

      // If the LogLevel is not found off is returned but we default to warn
      if (mLogLevel == CLogger::LogLevel::off
          && strcmp(LogLevel, "off") != 0)
        {
          mLogLevel = CLogger::LogLevel::warn;
        }
    }
  else
    {
      mLogLevel = CLogger::LogLevel::warn;
    }

  CLogger::setLevel(mLogLevel);
  CLogger::setLogDir(CDirEntry::dirName(mSummaryOutput) + "/EpiHiper");
  
  // DB Connection Defaults
  mDBConnection.name = "epihiper_db";
  mDBConnection.host = "localhost:5432";
  mDBConnection.user = "epihiper";
  mDBConnection.password.clear();
  mDBConnection.maxRecords = 100000;
  mDBConnection.connectionTimeout = 2;
  mDBConnection.connectionRetries = 15;
  mDBConnection.connectionMaxDelay = 500;

  pValue = json_object_get(pRoot, "dbName");

  if (json_is_string(pValue))
    {
      mDBConnection.name = json_string_value(pValue);
    }

  pValue = json_object_get(pRoot, "dbHost");

  if (json_is_string(pValue))
    {
      mDBConnection.host = json_string_value(pValue);
    }

  pValue = json_object_get(pRoot, "dbUser");

  if (json_is_string(pValue))
    {
      mDBConnection.user = json_string_value(pValue);
    }

  pValue = json_object_get(pRoot, "dbPassword");

  if (json_is_string(pValue))
    {
      mDBConnection.password = json_string_value(pValue);
    }

  pValue = json_object_get(pRoot, "dbMaxRecords");

  if (json_is_real(pValue))
    {
      mDBConnection.maxRecords = json_real_value(pValue);
    }

  pValue = json_object_get(pRoot, "dbConnectionTimeout");

  if (json_is_real(pValue))
    {
      mDBConnection.connectionTimeout = std::max< size_t >(2, json_real_value(pValue));
    }

  pValue = json_object_get(pRoot, "dbConnectionRetries");

  if (json_is_real(pValue))
    {
      mDBConnection.connectionRetries = json_real_value(pValue);
    }

  pValue = json_object_get(pRoot, "dbConnectionMaxDelay");

  if (json_is_real(pValue))
    {
      mDBConnection.connectionMaxDelay = json_real_value(pValue);
    }

  mDumpActiveNetwork.output = "";
  mDumpActiveNetwork.threshhold = -1.0;
  mDumpActiveNetwork.startTick = mStartTick;
  mDumpActiveNetwork.endTick = mEndTick;
  mDumpActiveNetwork.tickIncrement = 1;
  mDumpActiveNetwork.encoding = "text";

  json_t * pDump = json_object_get(pRoot, "dumpActiveNetwork");

  if (json_is_object(pDump))
    {
      pValue = json_object_get(pDump, "threshhold");

      if (json_is_real(pValue))
        {
          mDumpActiveNetwork.threshhold = json_real_value(pValue);
        }

      pValue = json_object_get(pDump, "output");

      if (json_is_string(pValue))
        mDumpActiveNetwork.output = json_string_value(pValue);
      else
        mDumpActiveNetwork.output = "contactNetwork";

      mDumpActiveNetwork.output = CDirEntry::resolve(mDumpActiveNetwork.output, mRunParameters, DefaultDir);

      if (!CDirEntry::exist(CDirEntry::dirName(mDumpActiveNetwork.output)))
        CDirEntry::createDir(CDirEntry::dirName(mDumpActiveNetwork.output));

      pValue = json_object_get(pDump, "startTick");

      if (json_is_real(pValue))
        {
          mDumpActiveNetwork.startTick = json_real_value(pValue);
        }

      valid &= mDumpActiveNetwork.startTick >= mStartTick;

      pValue = json_object_get(pDump, "endTick");

      if (json_is_real(pValue))
        {
          mDumpActiveNetwork.endTick = json_real_value(pValue);
        }

      valid &= mDumpActiveNetwork.endTick <= mEndTick
               && mDumpActiveNetwork.startTick <= mDumpActiveNetwork.endTick;

      pValue = json_object_get(pDump, "tickIncrement");

      if (json_is_real(pValue))
        {
          mDumpActiveNetwork.tickIncrement = json_real_value(pValue);
        }

      valid &= mDumpActiveNetwork.tickIncrement > 0;

      pValue = json_object_get(pDump, "encoding");

      if (json_is_string(pValue))
        {
          mDumpActiveNetwork.encoding = json_string_value(pValue);
        }

      valid &= mDumpActiveNetwork.encoding == "text" || mDumpActiveNetwork.encoding == "binary";
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
json_t * CSimConfig::loadJsonPreamble(const std::string & jsonFile, int flags)
{
  json_t * pRoot = NULL;

  if (jsonFile.empty())
    {
      CLogger::error("JSON Preamble file is not specified");
      return pRoot;
    }

  std::ifstream is(jsonFile.c_str());

  if (is.fail())
    {
      CLogger::error("JSON Preamble file: '" + jsonFile + "' cannot be opened.");
      return pRoot;
    }

  // Read only first line:
  std::string Line;
  std::getline(is, Line);

  json_error_t error;

  pRoot = json_loads(Line.c_str(), flags, &error);

  if (pRoot == NULL)
    {
      CLogger::error() << "JSON Preamble file: '" << jsonFile << "' error on line " << error.line << ": " << error.text << std::endl;
    }

  return pRoot;
}

// static
json_t * CSimConfig::loadJson(const std::string & jsonFile, int flags)
{
  json_t * pRoot = NULL;

  if (jsonFile.empty())
    {
      CLogger::error("JSON file is not specified.");
      return pRoot;
    }

  if (!CDirEntry::isFile(jsonFile))
    {
      CLogger::error() << "JSON file '" <<  jsonFile << "' not found.";
      return pRoot;
    }

  if (!CDirEntry::isReadable(jsonFile))
    {
      CLogger::error() << "JSON file '" <<  jsonFile << "' is not readable.";
      return pRoot;
    }

  json_error_t error;

  pRoot = json_load_file(jsonFile.c_str(), flags, &error);

  if (pRoot == NULL)
    {
      CLogger::error() << "JSON file: '" << jsonFile << "' error on line " << error.line << ": " << error.text << std::endl;
    }

  return pRoot;
}
