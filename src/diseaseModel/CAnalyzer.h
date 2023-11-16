// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2020 - 2023 Rector and Visitors of the University of Virginia 
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

#ifndef SRC_DISEASEMODEL_CANALYZER_H_
#define SRC_DISEASEMODEL_CANALYZER_H_

#include <string>

#include "diseaseModel/CModel.h"
#include "utilities/CLogger.h"

struct json_t;

class CAnalyzer
{
public:
  struct StateStatistics
  {
    CModel::state_t healthState = 0;
    size_t total = 0;
    size_t * histogramIn = NULL;
    size_t * histogramOut = NULL;
    double entryTickMean = 0;
    double entryTickSD = 0;
    double durationMean = 0;
    double durationSD = 0;
    std::set< CModel::state_t > exposedState = std::set< CModel::state_t >();
  };

  static void Init();

  static void Load(const std::string & jsonFile);

  static void LoadModel();

  static void Run();

  static void Release();

  static const size_t & Seed();

  static const std::string & StatusFile();
  
  virtual ~CAnalyzer();

private:
  CAnalyzer() = delete;

  CAnalyzer(const std::string & file);

  void loadModel();

  void run();

  static CAnalyzer * INSTANCE;

  size_t mSeed;
  size_t mMaxTick;
  size_t mSampleSize;
  StateStatistics * mData;
  CModel * mpModel;
  std::string mOutput;
  std::string mStatus;
  std::string mModel;
  CLogger::LogLevel mLogLevel;
};

#endif // SRC_DISEASEMODEL_CANALYZER_H_