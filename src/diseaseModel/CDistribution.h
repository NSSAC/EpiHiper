// BEGIN: Copyright 
// Copyright (C) 2019 - 2021 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#ifndef SRC_DISEASEMODEL_CDISTRIBUTION_H_
#define SRC_DISEASEMODEL_CDISTRIBUTION_H_

#include <vector>
#include <utility>

#include "utilities/CRandom.h"

struct json_t;

class CDistribution
{
public:
  enum struct Type {
   fixed,
   discrete,
   uniform,
   normal,
   gamma,
   __NONE
  };

  CDistribution();

  CDistribution(const CDistribution & src);

  virtual ~CDistribution();

  virtual void fromJSON(const json_t * json);

  const bool & isValid() const;

  unsigned int sample() const;

private:
  Type mType;
  std::vector< std::pair <double, unsigned int > > mDiscrete;
  std::vector< unsigned int > mUniformSet;
  unsigned int (CDistribution::*mpSample)() const;

  unsigned int mFixed;
  CRandom::uniform_int * mpUniformInt;
  CRandom::uniform_real * mpUniformReal;
  CRandom::normal * mpNormal;
  CRandom::gamma * mpGamma;

  bool mValid;

  unsigned int fixed() const;
  unsigned int discrete() const;
  unsigned int uniformSet() const;
  unsigned int uniformDiscrete() const;
  unsigned int normal() const;
  unsigned int gamma() const;
};

#endif /* SRC_DISEASEMODEL_CDISTRIBUTION_H_ */
