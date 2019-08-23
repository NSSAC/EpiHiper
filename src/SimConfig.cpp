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

#include <vector>

#include <fstream>
#include <iostream>
#include <jansson.h>
#include <limits>

#include "SimConfig.h"
#include "Misc.h"
#include "utilities/CDirEntry.h"

// static
SimConfig * SimConfig::INSTANCE(NULL);

// static
void SimConfig::load(const std::string & configFile)
{
  if (SimConfig::INSTANCE == NULL)
    {
      SimConfig::INSTANCE = new SimConfig(configFile);
    }
}

// static
void SimConfig::release()
{
  if (SimConfig::INSTANCE != NULL)
    {
      delete SimConfig::INSTANCE;
      SimConfig::INSTANCE = NULL;
    }
}

// static
bool SimConfig::isValid() { return SimConfig::INSTANCE->valid; }

// static
int SimConfig::getStartTick() { return SimConfig::INSTANCE->startTick; }

// static
int SimConfig::getEndTick() { return SimConfig::INSTANCE->endTick; }

// static
const std::string & SimConfig::getDiseaseModel() { return SimConfig::INSTANCE->diseaseModel; }

// static
const std::string & SimConfig::getContactNetwork() { return SimConfig::INSTANCE->contactNetwork; }

// static
const std::string & SimConfig::getInitialization() { return SimConfig::INSTANCE->initialization; }

// static
const std::string & SimConfig::getTraits() { return SimConfig::INSTANCE->traits; }

// static
const std::vector< std::string > & SimConfig::getPersonTraitDB() { return SimConfig::INSTANCE->personTraitDB; }

// static
const std::string & SimConfig::getOutput() { return SimConfig::INSTANCE->output; }

// static
const std::string & SimConfig::getIntervention() { return SimConfig::INSTANCE->intervention; }

// static
const size_t & SimConfig::getSeed() { return SimConfig::INSTANCE->seed; }

// static
const size_t & SimConfig::getReplicate() { return SimConfig::INSTANCE->replicate; }

// static
const SimConfig::db_connection & SimConfig::getDBConnection() { return SimConfig::INSTANCE->dbConnection; }

// constructor: parse JSON
SimConfig::SimConfig(const std::string& configFile)
  : valid(false)
  , runParameters(configFile)
  , modelScenario()
  , diseaseModel()
  , contactNetwork()
  , initialization()
  , traits()
  , personTraitDB()
  , startTick(std::numeric_limits< int >::min())
  , endTick(std::numeric_limits< int >::max())
  , output()
  , logFile()
  , intervention()
  , seed(-1)
  , replicate(-1)
  , dbConnection()
{
  if (runParameters.empty())
    {
      std::cerr << "Simulation configuration file is not specified" << std::endl;
      return;
    }

  CDirEntry::makePathAbsolute(runParameters, CDirEntry::getPWD());

  json_t * pRoot = loadJson(configFile, JSON_DECODE_INT_AS_REAL);

  if (pRoot == NULL)
    {
      return;
    }

  valid = true;

  json_t * pValue = json_object_get(pRoot, "modelScenario");

  if (json_is_string(pValue))
    {
      modelScenario = CDirEntry::resolve(json_string_value(pValue), runParameters);
    }

  valid &= !modelScenario.empty();

  pValue = json_object_get(pRoot, "output");

  if (json_is_string(pValue))
    {
      output = CDirEntry::resolve(json_string_value(pValue), runParameters);
    }

  pValue = json_object_get(pRoot, "startTick");

  if (json_is_real(pValue))
    {
      startTick = json_real_value(pValue);
    }

  valid &= startTick != std::numeric_limits< int >::min();

  pValue = json_object_get(pRoot, "endTick");

  if (json_is_real(pValue))
    {
      endTick = json_real_value(pValue);
    }

  valid &= endTick != std::numeric_limits< int >::max()
      && startTick < endTick;

  pValue = json_object_get(pRoot, "seed");

  if (json_is_real(pValue))
    {
      seed = json_real_value(pValue);
    }

  pValue = json_object_get(pRoot, "replicate");

  if (json_is_real(pValue))
    {
      replicate = json_real_value(pValue);
    }

  pValue = json_object_get(pRoot, "dbName");

  if (json_is_string(pValue))
    {
      dbConnection.name = json_string_value(pValue);
    }
  else
  	{
      dbConnection.name = "epihiper_db";
  	}

  pValue = json_object_get(pRoot, "dbHost");

  if (json_is_string(pValue))
    {
      dbConnection.host = json_string_value(pValue);
    }
  else
    {
      dbConnection.host = "localhost:5432";
    }

  pValue = json_object_get(pRoot, "dbUser");

  if (json_is_string(pValue))
    {
      dbConnection.user = json_string_value(pValue);
    }
  else
    {
      dbConnection.user = "epihiper";
    }


  pValue = json_object_get(pRoot, "dbPassword");

  if (json_is_string(pValue))
    {
      dbConnection.password = json_string_value(pValue);
    }
  else
    {
      dbConnection.password.clear();
    }

  json_decref(pRoot);

  valid &= loadScenario();
}

SimConfig::~SimConfig()
{
}

bool SimConfig::loadScenario()
{
  json_t * pRoot = loadJson(modelScenario, JSON_DECODE_INT_AS_REAL);

  if (pRoot == NULL) return false;

  json_t * pValue = json_object_get(pRoot, "contactNetwork");

  if (json_is_string(pValue))
    {
      contactNetwork = CDirEntry::resolve(json_string_value(pValue), modelScenario);
    }

  pValue = json_object_get(pRoot, "diseaseModel");

  if (json_is_string(pValue))
    {
      diseaseModel = CDirEntry::resolve(json_string_value(pValue), modelScenario);
    }

  pValue = json_object_get(pRoot, "initialization");

  if (json_is_string(pValue))
    {
      initialization = CDirEntry::resolve(json_string_value(pValue), modelScenario);
    }

  pValue = json_object_get(pRoot, "intervention");

  if (json_is_string(pValue))
    {
      intervention = CDirEntry::resolve(json_string_value(pValue), modelScenario);
    }

  pValue = json_object_get(pRoot, "traits");

  if (json_is_string(pValue))
    {
      traits = CDirEntry::resolve(json_string_value(pValue), modelScenario);
    }

  pValue = json_object_get(pRoot, "personTraitDB");

  if (json_is_array(pValue))
    for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i)
      {
        json_t * pDB = json_array_get(pValue, i);

        if (json_is_string(pDB))
          {
            personTraitDB.push_back(CDirEntry::resolve(json_string_value(pDB), modelScenario));
          }
      }

  json_decref(pRoot);

  return !contactNetwork.empty() &&
          !diseaseModel.empty() &&
          !intervention.empty() &&
          !initialization.empty() &&
          !traits.empty();
}

// static
json_t * SimConfig::loadJson(const std::string & jsonFile, int flags)
{
  json_t * pRoot = NULL;

  if (jsonFile.empty())
    {
      std::cerr << "JSON file is not specified" << std::endl;
      return pRoot;
    }

  std::ifstream is(jsonFile.c_str());

  if (is.fail())
    {
      std::cerr << "JSON file: '" << jsonFile << "' cannot be opened." << std::endl;
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
  is.seekg (0, is.end);
  size_t length = is.tellg();
  is.seekg (0, is.beg);

  char * buffer = new char [length + 1];
  is.read(buffer, length);
  buffer[length] = 0;

  pRoot = json_loads(buffer, flags, &error);
  delete [] buffer;

  if (pRoot == NULL)
    {
      std::cerr << "JSON file: '" << jsonFile << "' error on line " << error.line << ": " << error.text << std::endl;
    }

  return pRoot;
}

