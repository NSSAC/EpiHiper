#include <vector>

#include <fstream>
#include <iostream>
#include <jansson.h>

#include "SimConfig.h"
#include "Misc.h"

// constructor: parse JSON
SimConfig::SimConfig(const std::string& configFile)
  : valid(false)
  , diseaseModel()
  , contactNetwork()
  , initialization()
  , traits()
  , personTraitDB()
  , startTick(0)
  , endTick(0)
  , output()
  , logFile()
  , intervention()
{
  if (configFile.empty())
    {
      std::cerr << "Simulation configuration file is not specified" << std::endl;
      return;
    }

  json_t * pRoot = loadJson(configFile);

  if (pRoot == NULL)
    {
      return;
    }

  json_t * pValue = json_object_get(pRoot, "contactNetwork");

  if (json_is_string(pValue))
    contactNetwork = json_string_value(pValue);

  pValue = json_object_get(pRoot, "diseaseModel");

  if (json_is_string(pValue))
    diseaseModel = json_string_value(pValue);

  pValue = json_object_get(pRoot, "initialization");

  if (json_is_string(pValue))
    initialization = json_string_value(pValue);

  pValue = json_object_get(pRoot, "intervention");

  if (json_is_string(pValue))
    intervention = json_string_value(pValue);

  pValue = json_object_get(pRoot, "traits");

  if (json_is_string(pValue))
    traits = json_string_value(pValue);

  pValue = json_object_get(pRoot, "personTraitDB");

  if (json_is_string(pValue))
    personTraitDB = json_string_value(pValue);

  pValue = json_object_get(pRoot, "output");

  if (json_is_string(pValue))
    output = json_string_value(pValue);

  pValue = json_object_get(pRoot, "startTick");

  if (json_is_integer(pValue))
    startTick = json_integer_value(pValue);

  pValue = json_object_get(pRoot, "endTick");

  if (json_is_integer(pValue))
    endTick = json_integer_value(pValue);
}

SimConfig::~SimConfig()
{
}

void SimConfig::validate()
{
  valid = true;
  std::ifstream f(contactNetwork.data());
  if (!f.good())
    valid = false;
}

json_t * SimConfig::loadJson(const std::string & jsonFile) const
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

