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

#ifndef SRC_DISEASEMODEL_DISTRIBUTION_H_
#define SRC_DISEASEMODEL_DISTRIBUTION_H_

#include <vector>
#include <utility>

#include "utilities/Random.h"

struct json_t;

class Distribution
{
public:
  enum struct Type {
   fixed,
   discrete,
   uniform,
   normal,
   __NONE
  };

  Distribution();

  Distribution(const Distribution & src);

  virtual ~Distribution();

  void fromJSON(const json_t * json);

  const bool & isValid() const;

  unsigned int sample() const;

private:
  Type mType;
  std::vector< std::pair <double, unsigned int > > mDiscrete;
  std::vector< unsigned int > mUniformSet;
  unsigned int (Distribution::*mpSample)() const;

  unsigned int mFixed;
  Random::uniform_int * mpUniformInt;
  Random::uniform_real * mpUniformReal;
  Random::normal * mpNormal;

  bool mValid;

  unsigned int fixed() const;
  unsigned int discrete() const;
  unsigned int uniformSet() const;
  unsigned int uniformDiscrete() const;
  unsigned int normal() const;
};

#endif /* SRC_DISEASEMODEL_DISTRIBUTION_H_ */
