// BEGIN: Copyright 
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#ifndef SRC_DISEASEMODEL_CTRANSMISSION_H_
#define SRC_DISEASEMODEL_CTRANSMISSION_H_

#include <map>

#include "utilities/CAnnotation.h"
#include "diseaseModel/CFactorOperation.h"
#include "plugins/epiHiperPlugin.h"

struct json_t;
class CHealthState;

class CTransmission: public CAnnotation, public CCustomMethod< epiHiperPlugin::transmission_propensity >
{
public:
  static double defaultMethod(const CTransmission * pTransmission, const CEdge * pEdge);

  CTransmission();

  CTransmission(const CTransmission & src);

  virtual ~CTransmission();

  void fromJSON(const json_t * json, const std::map< std::string, CHealthState * > & states);

  const std::string & getId() const;

  const CHealthState * getEntryState() const;

  const CHealthState * getContactState() const;

  const CHealthState * getExitState() const;

  const double & getTransmissibility() const;

  const bool & isValid() const;

  void updateSusceptibilityFactor(double & factor) const;

  void updateInfectivityFactor(double & factor) const;

  double propensity(const CEdge * pEdge) const;

private:
  std::string mId;
  const CHealthState * mpEntryState;
  const CHealthState * mpExitState;
  const CHealthState * mpContactState;
  double mTransmissibility;
  CFactorOperation mSusceptibilityFactorOperation;
  CFactorOperation mInfectivityFactorOperation;
  bool mValid;
};

#endif /* SRC_DISEASEMODEL_CTRANSMISSION_H_ */
