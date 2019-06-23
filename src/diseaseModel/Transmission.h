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

#ifndef SRC_DISEASEMODEL_TRANSMISSION_H_
#define SRC_DISEASEMODEL_TRANSMISSION_H_

#include "utilities/Annotation.h"
#include "FactorOperation.h"

struct json_t;
class State;

class Transmission: public Annotation
{
public:
  Transmission();

  Transmission(const Transmission & src);

  virtual ~Transmission();

  void fromJSON(const json_t * json);

  const std::string & getId() const;

  const bool & isValid() const;

private:
  std::string mId;
  const State * mpEntryState;
  const State * mpExitState;
  const State * mpContactState;
  double mTransmissibility;
  FactorOperation mSusceptibilityFactorOperation;
  FactorOperation mInfectivityFactorOperation;
  bool mValid;
};

#endif /* SRC_DISEASEMODEL_TRANSMISSION_H_ */
