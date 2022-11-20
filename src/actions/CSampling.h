// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
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

#ifndef SRC_ACTIONS_CSAMPLING_H_
#define SRC_ACTIONS_CSAMPLING_H_

#include "utilities/CCommunicate.h"
#include "utilities/CContext.h"
#include "sets/CSetContent.h"

struct json_t;
class CActionEnsemble;
class CVariable;
class CSampling
{
  class CSampled : public CSetContent
  {
  public:
    CSampled();

    CSampled(const CSampled & src);

    virtual ~CSampled();

    virtual CSetContent * copy() const override;

    virtual void fromJSON(const json_t *json) override;
  };

public:
  enum struct Type
  {
    relativeIndividual,
    relativeGroup,
    absolute,
  };

  CSampling();

  CSampling(const CSampling & src);

  CSampling(const json_t * json);

  virtual ~CSampling();

  virtual void fromJSON(const json_t *json);

  const bool & isValid() const;

  bool isEmpty() const;
  
  void process(const CSetContent & targets);

private:
  int broadcastCount();

  CCommunicate::ErrorCode receiveCount(std::istream & is, int sender);

  void determineThreadLimits();

  Type mType;
  CVariable * mpVariable;
  double mPercentage;
  size_t mCount;
  CActionEnsemble * mpSampled;
  CActionEnsemble * mpNotSampled;
  CSetContent const * mpTargets;
  CSampled mSampledTargets;
  CSampled mNotSampledTargets;
  CContext< size_t > mLocalLimit;
  size_t * mpCommunicateBuffer;

  bool mValid;
};

#endif /* SRC_ACTIONS_CSAMPLING_H_ */
