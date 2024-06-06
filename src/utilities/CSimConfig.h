// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2024 Rector and Visitors of the University of Virginia 
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

#ifndef SIM_CONFIG_H
#define SIM_CONFIG_H

#include <vector>
#include <map>
#include <string>

#include "utilities/CLogger.h"
#include "utilities/CDirEntry.h"

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

  struct dump_active_network
  {
    std::string output;
    double threshold = -1.0;
    int startTick;
    int endTick;
    int tickIncrement;
    std::string encoding;
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
  std::vector< std::string > mPlugins;
  size_t mSeed;
  std::map< int, size_t > mReseed;
  size_t mReplicate;
  size_t mPartitionEdgeLimit;
  CLogger::LogLevel mLogLevel;
  db_connection mDBConnection;
  dump_active_network mDumpActiveNetwork;

private:
  static CSimConfig * INSTANCE;

  CSimConfig() = delete;
  CSimConfig(const std::string & configFile);
  ~CSimConfig();

  bool loadScenario();

public:
  static void load(const std::string & configFile);
  static void clear();

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
  static const std::vector < std::string > & getPlugins();
  static const size_t & getSeed();
  static const std::map< int, size_t> & getReseed();
  static const size_t & getReplicate();
  static const size_t & getPartitionEdgeLimit();
  static CLogger::LogLevel getLogLevel();
  static const db_connection & getDBConnection();
  static const dump_active_network & getDumpActiveNetwork();
  template < class Severity >
  static json_t * loadJson(const std::string & jsonFile, int flags);
  static json_t * loadJsonPreamble(const std::string & jsonFile, int flags);
  static std::string jsonToString(const json_t * pJson);
};

#ifdef HAVE_SYS_TYPES_H
# undef HAVE_SYS_TYPES_H
#endif

#include <jansson.h>

template < class Severity >
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
      Severity("JSON file '{}' not found.", jsonFile);
      return pRoot;
    }

  if (!CDirEntry::isReadable(jsonFile))
    {
      Severity("JSON file '{}' is not readable.", jsonFile);
      return pRoot;
    }

  json_error_t error;

  pRoot = json_load_file(jsonFile.c_str(), flags, &error);

  if (pRoot == NULL)
    {
      Severity("JSON file: '{}' error on line {}: {}", jsonFile, error.line, error.text);
    }

  return pRoot;
}

#endif
