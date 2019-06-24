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

#ifndef SRC_DISEASEMODEL_MODEL_H_
#define SRC_DISEASEMODEL_MODEL_H_

#include <map>
#include <string>
#include <vector>

#include "utilities/Annotation.h"

class State;
class Transmission;
class Progression;

struct json_t;

class Model: public Annotation
{
public:
  Model();

  Model(const Model & src);

  virtual ~Model();

  void fromJSON(const json_t * json);

  const std::string & getId() const;

  const bool & isValid() const;

private:
  std::map< std::string, State > mStates;
  State const * mpInitialState;
  std::vector< Transmission > mTransmissions;
  std::vector< Progression > mProgressions;
  bool mValid;
};

#endif /* SRC_DISEASEMODEL_MODEL_H_ */
