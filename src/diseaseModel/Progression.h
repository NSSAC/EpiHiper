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

#ifndef SRC_DISEASEMODEL_PROGRESSION_H_
#define SRC_DISEASEMODEL_PROGRESSION_H_

#include <map>

#include "utilities/Annotation.h"
#include "FactorOperation.h"
#include "Distribution.h"

struct json_t;
class State;

class Progression: public Annotation
{
public:
  Progression();

  Progression(const Progression & src);

  virtual ~Progression();

  void fromJSON(const json_t * json, const std::map< std::string, State> & states);

  const std::string & getId() const;

  const bool & isValid() const;

private:
  std::string mId;
  const State * mpEntryState;
  const State * mpExitState;
  double mProbability;
  Distribution mDwellTime;
  FactorOperation mSusceptibilityFactorOperation;
  FactorOperation mInfectivityFactorOperation;
  bool mValid;
};

#endif /* SRC_DISEASEMODEL_PROGRESSION_H_ */
