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

#ifndef SRC_DISEASEMODEL_CPROGRESSION_H_
#define SRC_DISEASEMODEL_CPROGRESSION_H_

#include <map>

#include "utilities/CAnnotation.h"
#include "CDistribution.h"
#include "CFactorOperation.h"

struct json_t;
class CHealthState;

class CProgression: public CAnnotation
{
public:
  CProgression();

  CProgression(const CProgression & src);

  virtual ~CProgression();

  void fromJSON(const json_t * json, const std::map< std::string, CHealthState * > & states);

  const std::string & getId() const;

  const CHealthState * getEntryState() const;

  const CHealthState * getExitState() const;

  const double & getPropability() const;

  unsigned int getDwellTime() const;

  void updateSusceptibilityFactor(double & factor) const;

  void updateInfectivityFactor(double & factor) const;

  const bool & isValid() const;

private:
  std::string mId;
  const CHealthState * mpEntryState;
  const CHealthState * mpExitState;
  double mProbability;
  CDistribution mDwellTime;
  CFactorOperation mSusceptibilityFactorOperation;
  CFactorOperation mInfectivityFactorOperation;
  bool mValid;
};

#endif /* SRC_DISEASEMODEL_CPROGRESSION_H_ */
