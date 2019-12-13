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

#ifndef SRC_ACTIONS_CSAMPLING_H_
#define SRC_ACTIONS_CSAMPLING_H_

#include "utilities/CCommunicate.h"
#include "sets/CSetContent.h"

struct json_t;
class CActionEnsemble;

class CSampling
{
  class CSampled : public CSetContent
  {
  public:
    CSampled();

    CSampled(const CSampled & src);

    virtual ~CSampled();

    virtual CSetContent * copy() const;
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

  void process(const CSetContent & targets);

private:
  int broadcastCount();

  CCommunicate::ErrorCode receiveCount(std::istream & is, int sender);

  Type mType;
  double mPercentage;
  size_t mCount;
  CActionEnsemble * mpSampled;
  CActionEnsemble * mpNotSampled;
  CSetContent const * mpTargets;
  CSampled mSampledTargets;
  CSampled mNotSampledTargets;
  size_t mLocalLimit;
  size_t * mpCommunicateBuffer;

  bool mValid;
};

#endif /* SRC_ACTIONS_CSAMPLING_H_ */
