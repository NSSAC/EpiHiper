// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2023 Rector and Visitors of the University of Virginia 
//  
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal 
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions: 
//  
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software. 
//  
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE 
// END: Copyright 

#include <cmath>
#include <limits>
#include <iostream>
#include <cstdlib>
#include <jansson.h>

#include "utilities/CArgs.h"

#include "network/CNetwork.h"
#include "traits/CTrait.h"
#include "utilities/CCommunicate.h"
#include "utilities/CSimConfig.h"
#include "utilities/CStatus.h"
#include "utilities/CDirEntry.h"
#include "utilities/CLogger.h"

std::string ContactNetwork;
std::string OutputDirectory;
std::string Status;

int Parts(std::numeric_limits< int >::min());

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

  if (CDirEntry::exist("/output")
      && CDirEntry::isWritable("/output"))
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

  if (CDirEntry::exist("/job")
      && CDirEntry::isWritable("/job"))
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

int main(int argc, char * argv[])
{
  int EXIT = EXIT_SUCCESS;

  CLogger::init();
  CCommunicate::init(argc, argv);
  bool ArgumentsValid = CArgs::parseArgs(argc, argv);

  if (CCommunicate::MPIRank == 0)
    CArgs::printWhoAmI();

  if (!ArgumentsValid)
    {
      if (CCommunicate::MPIRank == 0)
        CArgs::printUsage();

      CCommunicate::abort(CCommunicate::ErrorCode::InvalidArguments);
      CCommunicate::finalize();
      CLogger::release();

      exit(EXIT_FAILURE);
    }

  if (!loadJson(CArgs::getConfig()))
    {
      CLogger::error("Invalid paramters in: " + CArgs::getConfig());

      CCommunicate::abort(CCommunicate::ErrorCode::InvalidArguments);
      CCommunicate::finalize();
      CLogger::release();

      exit(EXIT_FAILURE);
    }

  CTrait::init();
  CNetwork All;
  All.loadJsonPreamble(ContactNetwork);
  All.partition(Parts, true, OutputDirectory);

  if (CLogger::hasErrors())
    {
      EXIT = EXIT_FAILURE;
    }

  CCommunicate::finalize();
  CLogger::release();

  exit(EXIT);
}
