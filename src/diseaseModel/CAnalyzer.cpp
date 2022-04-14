// BEGIN: Copyright 
// Copyright (C) 2020 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include <fstream>
#include <jansson.h>

#include "diseaseModel/CAnalyzer.h"
#include "diseaseModel/CTransmission.h"
#include "diseaseModel/CProgression.h"
#include "diseaseModel/CHealthState.h"
#include "utilities/CCommunicate.h"
#include "utilities/CLogger.h"
#include "utilities/CSimConfig.h"
#include "utilities/CDirEntry.h"

// static
void CAnalyzer::Init()
{}

// static
void CAnalyzer::Load(const std::string & file)
{
  INSTANCE = new CAnalyzer(file);
}

// static
void CAnalyzer::Run()
{
  INSTANCE->run();
}

// static
void CAnalyzer::Release()
{
  if (INSTANCE != NULL)
    delete INSTANCE;

  INSTANCE = NULL;
}

// static
const size_t & CAnalyzer::Seed()
{
  return INSTANCE->mSeed;
}

// static 
const std::string & CAnalyzer::StatusFile()
{
  return INSTANCE->mStatus;
}

// virtual
CAnalyzer::~CAnalyzer()
{
  if (mData != NULL)
    {
      StateStatistics * pIt = mData;
      StateStatistics * pEnd = pIt + mpModel->getStateCount();

      for (; pIt != pEnd; ++pIt)
        {
          delete [] pIt->histogramIn;
          delete [] pIt->histogramOut;
        }

      delete[] mData;
    }

  if (mpModel != NULL)
    delete mpModel;
}

CAnalyzer::CAnalyzer(const std::string & jsonFile)
  : mSeed(-1)
  , mMaxTick(100)
  , mSampleSize(100000)
  , mData(NULL)
  , mpModel(NULL)
  , mOutput("self://./output.csv")
  , mStatus("self://./analyzer.status.json")
  , mLogLevel(CLogger::LogLevel::warn)
{
  /*
  {
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "Model Analyzer Configuration",
    "description": "The configuration file for the model analyzer",
    "type": "object",
    "required": [
      "epiHiperSchema",
      "diseaseModel"
    ],
    "properties": {
      "epiHiperSchema": {
        "type": "string",
        "pattern": "^.* /modelScenarioSchema.json$"
      },
      "diseaseModel": {
        "description": "Filename of the disease model in a json file.",
        "allOf": [
          {"$ref": "./typeRegistry.json#/definitions/localPath"},
          {"$ref": "./typeRegistry.json#/definitions/jsonFormat"}
        ]
      },
      "output": {
        "description": "Path + name of the output file",
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
        "description": "The seed for the random number generator (default: random seeding)",
        "$ref": "./typeRegistry.json#/definitions/nonNegativeInteger"
      },
      "sampleSize": {
        "description": "The number of samples taken for each exit state of a transmission (default: 100,000)",
        "type": "number",
        "minimum": 0,
        "multipleOf": 1.0
      },
      "maxTick": {
        "description": "The maximum tick considered when analyzing the model (default: 100)",
        "type": "number",
        "minimum": 0,
        "multipleOf": 1.0
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
      }
    }
  }
  */

  json_t * pRoot = CSimConfig::loadJson(jsonFile, JSON_DECODE_INT_AS_REAL);

  if (pRoot == NULL)
    {
      return;
    }

  json_t * pValue;

  std::string DefaultDir;

  if (CDirEntry::exist("/output")
      && CDirEntry::isWritable("/output"))
    DefaultDir = "/output";
  else
    DefaultDir = ".";

  pValue = json_object_get(pRoot, "output");

  if (json_is_string(pValue))
    mOutput = json_string_value(pValue);

  mOutput = CDirEntry::resolve(mOutput, jsonFile, DefaultDir);

  if (!CDirEntry::exist(CDirEntry::dirName(mOutput)))
    CDirEntry::createDir(CDirEntry::dirName(mOutput));

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
  CLogger::setLogDir(CDirEntry::dirName(mOutput) + "/EpiHiperModelAnalyzer");

  pValue = json_object_get(pRoot, "diseaseModel");

  if (json_is_string(pValue))
    {
      std::string DiseaseModel = CDirEntry::resolve(json_string_value(pValue), jsonFile);
      mpModel = new CModel(DiseaseModel);
    }
  else
    {
      CLogger::error() << "CAnalyzer: Missing property 'diseaseModel'.";
    }

  if (CDirEntry::exist("/job")
      && CDirEntry::isWritable("/job"))
    DefaultDir = "/job";
  else
    DefaultDir = ".";

  pValue = json_object_get(pRoot, "status");

  if (json_is_string(pValue))
    mStatus = json_string_value(pValue);

  mStatus = CDirEntry::resolve(mStatus, jsonFile, DefaultDir);

  if (!CDirEntry::exist(CDirEntry::dirName(mStatus)))
    CDirEntry::createDir(CDirEntry::dirName(mStatus));

  pValue = json_object_get(pRoot, "sampleSize");

  if (json_is_real(pValue))
    {
      mSampleSize = json_real_value(pValue);
    }

  pValue = json_object_get(pRoot, "maxTick");

  if (json_is_real(pValue))
    {
      mMaxTick = json_real_value(pValue);
    }

  pValue = json_object_get(pRoot, "seed");

  if (json_is_real(pValue))
    {
      mSeed = json_real_value(pValue);
    }

  if (!CLogger::hasErrors())
    {
      mData = new StateStatistics[mpModel->getStateCount()];
      StateStatistics * pIt = mData;
      StateStatistics * pEnd = pIt + mpModel->getStateCount();

      for (CModel::state_t s = 0; pIt != pEnd; ++pIt, ++s)
        {
          pIt->healthState = s;
          pIt->histogramIn = new size_t[mMaxTick + 1];
          memset(pIt->histogramIn, 0, (mMaxTick + 1) * sizeof(size_t));
          pIt->histogramOut = new size_t[mMaxTick + 1];
          memset(pIt->histogramOut, 0, (mMaxTick + 1) * sizeof(size_t));
        }
    }
}

void CAnalyzer::run()
{
  // Determine all exit states of transmissions
  std::vector< CTransmission >::const_iterator itTransmission = mpModel->getTransmissions().begin();
  std::vector< CTransmission >::const_iterator endTransmission = mpModel->getTransmissions().end();
  std::set< const CHealthState * > Exposed;

  for (; itTransmission != endTransmission; ++itTransmission)
    {
      Exposed.insert(itTransmission->getExitState());
    }

  // For each  exit state sample transmission  sampleSize times up to maxTick
  std::set< const CHealthState * >::const_iterator itExposed = Exposed.begin();
  std::set< const CHealthState * >::const_iterator endExposed = Exposed.end();

  size_t MaxTick = 0;

  for (; itExposed != endExposed; ++itExposed)
    {
      CModel::state_t ExposedType = mpModel->stateToType(*itExposed);

      for (size_t i = 0; i != mSampleSize; ++i)
        {
          const CHealthState * pState = *itExposed;
          size_t Tick = 0;

          while (pState != NULL && Tick <= mMaxTick)
            {
              // Record the current state and time
              CModel::state_t StateType = mpModel->stateToType(pState);
             
              StateStatistics & Statistics = mData[StateType];
              Statistics.exposedState.insert(ExposedType);
              ++Statistics.total;
              ++Statistics.histogramIn[Tick];
              
              double delta = Tick - Statistics.entryTickMean;
              Statistics.entryTickMean += delta/Statistics.total;
              Statistics.entryTickSD += delta * (Tick - Statistics.entryTickMean);

              // Advance the state according to the possible progressions.
              const CProgression * pProgression = mpModel->nextProgression(StateType);

              if (pProgression != NULL)
                {
                  pState = pProgression->getExitState();
                  unsigned int Duration = pProgression->getDwellTime();
                  Tick += Duration;
                  
                  if (Tick <= mMaxTick)
                    ++Statistics.histogramOut[Tick];

                  delta = Duration - Statistics.durationMean;
                  Statistics.durationMean += delta/Statistics.total;
                  Statistics.durationSD += delta * (Duration - Statistics.durationMean);
                }
              else
                {
                  if (Tick > MaxTick)
                    MaxTick = Tick;

                  pState = NULL;
                }
            }
        }
    }

  // Write the result to a csv file
  std::ofstream os(mOutput);

  // Header row
  os << "State,Total,Relative,Entry Tick Mean,Entry Tick SD,Duration,Duration SD,Tick";

  for (size_t i = 0; i <= MaxTick; ++i)
    os << "," << i;

  os << std::endl;

  StateStatistics * pIt = mData;
  StateStatistics * pEnd = pIt + mpModel->getStateCount();

  for (; pIt != pEnd; ++pIt)
    {
      os << mpModel->stateFromType(pIt->healthState)->getId() << "," << pIt->total << "," << ((double )pIt->total)/(pIt->exposedState.size() * mSampleSize) << "," << pIt->entryTickMean << "," << pIt->entryTickSD/pIt->total << "," << pIt->durationMean << "," << pIt->durationSD/pIt->total << ",in" ;

      for (size_t i = 0; i <= MaxTick; ++i)
        os << "," << pIt->histogramIn[i];

      os << std::endl;

      os <<  ",,,,,,,current" ;

      size_t Current = 0;

      for (size_t i = 0; i <= MaxTick; ++i)
        {
          Current += pIt->histogramIn[i] - pIt->histogramOut[i];
          os << "," << Current;
        }

      os << std::endl;

      os <<  ",,,,,,,out" ;

      for (size_t i = 0; i <= MaxTick; ++i)
        os << "," << pIt->histogramOut[i];

      os << std::endl;

    }
}
