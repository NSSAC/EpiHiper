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

#include <cmath>
#include <limits>
#include <iostream>
#include <cstdlib>
#include <getopt.h>
#include <jansson.h>

#include "network/CNetwork.h"
#include "traits/CTrait.h"
#include "utilities/CCommunicate.h"
#include "utilities/CSimConfig.h"
#include "utilities/CStatus.h"
#include "utilities/CDirEntry.h"

std::string ContactNetwork;
std::string OutputDirectory;
std::string Status;
std::string Config;

int Parts(std::numeric_limits< int >::min());

bool parseArgs(int argc, char *argv[])
{
  const char* const short_opts = "c";

  const option long_opts[] =
  {
   {"config", required_argument, nullptr, 'c'},
   {nullptr, no_argument, nullptr, 0}
  };

  while (true)
    {
      const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

      if (-1 == opt)
        break;

      switch(opt)
      {
        case 'c':
          Config = std::string(optarg);
          break;

        case '?':
        default:
          return false;
      }
    }

  return true;
}

void printUsage()
{
  std::cout << "\nUsage:\n\n"
    "Partition --config <configFilename>\n"
     "--config: required string specifying EpiHiper configuration file\n";
}

bool loadJson(const std::string & file)
{
  /*
  "required": [
    "epiHiperSchema",
    "contactNetwork",
    "numberOfParts"
  ],
  "properties": {
    "epiHiperSchema": {
      "type": "string",
      "enum": ["https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/splitSchema.json"]
    },
    "contactNetwork": {
      "description": "Filename of the contact network",
      "$ref": "./typeRegistry.json#/definitions/localPath"
    },
    "numberOfParts": {
      "description": "The number of part to create.",
      "type": "number",
      "minimum": 0,
      "multipleOf": 1.0
    },
    "outputDirectory": {
      "description": "Output Directory for the created parts",
      "default": "/output",
      "$ref": "./typeRegistry.json#/definitions/localPath"
    },
    "status": {
      "description": "Path + name of the output SciDuct status file",
      "allOf": [
        {"$ref": "./typeRegistry.json#/definitions/localPath"},
        {"$ref": "./typeRegistry.json#/definitions/jsonFormat"}
      ]
    }
  }
   */

  json_t * pRoot = CSimConfig::loadJson(file, JSON_DECODE_INT_AS_REAL);

  if (pRoot == NULL)
    {
      return false;
    }

  bool success = true;

  json_t * pValue = json_object_get(pRoot, "contactNetwork");

  if (json_is_string(pValue))
    {
      ContactNetwork = CDirEntry::resolve(json_string_value(pValue), file);
    }

  success &= !ContactNetwork.empty();

  pValue = json_object_get(pRoot, "numberOfParts");

  if (json_is_real(pValue))
    {
      Parts = std::round(json_real_value(pValue));
    }

  success &= Parts != std::numeric_limits< int >::min();

  std::string DefaultDir;

  if (CDirEntry::exist("/output") &&
      CDirEntry::isWritable("/output"))
    DefaultDir = "/output";
  else
    DefaultDir = ".";

  pValue = json_object_get(pRoot, "outputDirectory");

  if (json_is_string(pValue))
    OutputDirectory = json_string_value(pValue);
  else
    OutputDirectory = "";

  OutputDirectory = CDirEntry::resolve(OutputDirectory, file, DefaultDir);

  if (!CDirEntry::exist(CDirEntry::dirName(OutputDirectory)))
    CDirEntry::createDir(CDirEntry::dirName(OutputDirectory));

  if (CDirEntry::exist("/job") &&
      CDirEntry::isWritable("/job"))
    DefaultDir = "/job";
  else
    DefaultDir = ".";

  pValue = json_object_get(pRoot, "status");

  if (json_is_string(pValue))
    Status = json_string_value(pValue);
  else
    Status = "sciduct.status.json";

  Status = CDirEntry::resolve(Status, file, DefaultDir);

  if (!CDirEntry::exist(CDirEntry::dirName(Status)))
    CDirEntry::createDir(CDirEntry::dirName(Status));

  json_decref(pRoot);

  return success;
}

int main(int argc, char *argv[])
{
  CCommunicate::init(argc, argv);

  if (CCommunicate::MPIRank == 0) {
      std::cout << "EpiHiper version 0.0.1 (2019.06.14)" << std::endl;
  }

  if (argc < 3 || ! parseArgs(argc, argv))
    {
      if (CCommunicate::MPIRank == 0)
        {
          printUsage();
        }

      CCommunicate::abort(CCommunicate::ErrorCode::InvalidArguments);
      CCommunicate::finalize();

      exit(EXIT_FAILURE);
    }

  if (!loadJson(Config))
    {
      std::cerr << "Invalid paramters in: " << Config << std::endl;

      CCommunicate::abort(CCommunicate::ErrorCode::InvalidArguments);
      CCommunicate::finalize();

      exit(EXIT_FAILURE);
    }

  CTrait::init();
  CNetwork All(ContactNetwork);
  All.partition(Parts, true, OutputDirectory);
}




