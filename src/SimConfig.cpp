#include <vector>

#include <fstream>
#include <iostream>
#include <jansson.h>
#include <limits>

#include "SimConfig.h"
#include "Misc.h"
#include "utilities/DirEntry.h"

// static
SimConfig * SimConfig::CONFIG(NULL);

// static
void SimConfig::init(const std::string & configFile)
{
  if (SimConfig::CONFIG == NULL)
    {
      SimConfig::CONFIG = new SimConfig(configFile);
    }
}

// static
void SimConfig::release()
{
  if (SimConfig::CONFIG != NULL)
    {
      delete SimConfig::CONFIG;
      SimConfig::CONFIG = NULL;
    }
}

// static
bool SimConfig::isValid() { return SimConfig::CONFIG->valid; }

// static
int SimConfig::getStartTick() { return SimConfig::CONFIG->startTick; }

// static
int SimConfig::getEndTick() { return SimConfig::CONFIG->endTick; }

// static
const std::string& SimConfig::getDiseaseModel() { return SimConfig::CONFIG->diseaseModel; }

// static
const std::string& SimConfig::getContactNetwork() { return SimConfig::CONFIG->contactNetwork; }

// static
const std::string& SimConfig::getInitialization() { return SimConfig::CONFIG->initialization; }

// static
const std::string& SimConfig::getTraits() { return SimConfig::CONFIG->traits; }

// static
const std::string& SimConfig::getPersonTraitDB() { return SimConfig::CONFIG->personTraitDB; }

// static
const std::string& SimConfig::getOutput() { return SimConfig::CONFIG->output; }

// static
const std::string& SimConfig::getIntervention() { return SimConfig::CONFIG->intervention; }

// constructor: parse JSON
SimConfig::SimConfig(const std::string& configFile)
  : valid(false)
  , runParameters(configFile)
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
{
  if (runParameters.empty())
    {
      std::cerr << "Simulation configuration file is not specified" << std::endl;
      return;
    }

  DirEntry::makePathAbsolute(runParameters, DirEntry::getPWD());

  json_t * pRoot = loadJson(configFile);

  if (pRoot == NULL)
    {
      return;
    }

  json_t * pValue = json_object_get(pRoot, "contactNetwork");

  if (json_is_string(pValue))
    {
      contactNetwork = json_string_value(pValue);
      DirEntry::makePathAbsolute(contactNetwork, runParameters);
    }

  pValue = json_object_get(pRoot, "diseaseModel");

  if (json_is_string(pValue))
    {
      diseaseModel = json_string_value(pValue);
      DirEntry::makePathAbsolute(diseaseModel, runParameters);
    }

  pValue = json_object_get(pRoot, "initialization");

  if (json_is_string(pValue))
    {
      initialization = json_string_value(pValue);
      DirEntry::makePathAbsolute(initialization, runParameters);
    }

  pValue = json_object_get(pRoot, "intervention");

  if (json_is_string(pValue))
    {
      intervention = json_string_value(pValue);
      DirEntry::makePathAbsolute(intervention, runParameters);
    }

  pValue = json_object_get(pRoot, "traits");

  if (json_is_string(pValue))
    {
      traits = json_string_value(pValue);
      DirEntry::makePathAbsolute(traits, runParameters);
    }

  pValue = json_object_get(pRoot, "personTraitDB");

  if (json_is_string(pValue))
    {
      personTraitDB = json_string_value(pValue);
      DirEntry::makePathAbsolute(personTraitDB, runParameters);
    }

  pValue = json_object_get(pRoot, "output");

  if (json_is_string(pValue))
    {
      output = json_string_value(pValue);
      DirEntry::makePathAbsolute(output, runParameters);
    }

  pValue = json_object_get(pRoot, "startTick");

  if (json_is_integer(pValue))
    {
      startTick = json_integer_value(pValue);
    }

  pValue = json_object_get(pRoot, "endTick");

  if (json_is_integer(pValue))
    {
      endTick = json_integer_value(pValue);
    }

  valid = !contactNetwork.empty() &&
          !diseaseModel.empty() &&
          !intervention.empty() &&
          !initialization.empty() &&
          !traits.empty() &&
          startTick != std::numeric_limits< int >::min() &&
          endTick != std::numeric_limits< int >::max() &&
          startTick < endTick;
}

SimConfig::~SimConfig()
{
}

// static
json_t * SimConfig::loadJson(const std::string & jsonFile)
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

  // get length of file:
  is.seekg (0, is.end);
  size_t length = is.tellg();
  is.seekg (0, is.beg);

  char * buffer = new char [length + 1];
  is.read(buffer, length);
  buffer[length] = 0;

  json_error_t error;

  pRoot = json_loads(buffer, 0, &error);
  delete [] buffer;

  if (pRoot == NULL)
    {
      std::cerr << "JSON file: '" << jsonFile << "' error on line " << error.line << ": " << error.text << std::endl;
    }

  return pRoot;
}

