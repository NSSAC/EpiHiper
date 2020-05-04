// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#ifndef SRC_NETWORK_CEDGE_H_
#define SRC_NETWORK_CEDGE_H_

#include <iostream>

#include "EpiHiperConfig.h"
#include "traits/CTraitData.h"
#include "math/CValueInterface.h"

class CNode;
class CMetadata;

class CEdge
{
public:
  static bool HasLocationId;
  static bool HasEdgeTrait;
  static bool HasActiveField;
  static bool HasWeightField;
  static CEdge getDefault();

  CEdge();
  CEdge(const CEdge & src);
  virtual ~CEdge();

  void toBinary(std::ostream & os) const;
  void fromBinary(std::istream & is);

  bool setTargetActivity(CTraitData::value value, CValueInterface::pOperator pOperator, const CMetadata & metadata);
  bool setSourceActivity(CTraitData::value value, CValueInterface::pOperator pOperator, const CMetadata & metadata);
  bool setEdgeTrait(CTraitData::value value, CValueInterface::pOperator pOperator, const CMetadata & metadata);
  bool setActive(bool value, CValueInterface::pOperator pOperator, const CMetadata & metadata);
  bool setWeight(double value, CValueInterface::pOperator pOperator, const CMetadata & metadata);

  size_t targetId;
  CTraitData::base targetActivity;
  size_t sourceId;
  CTraitData::base sourceActivity;
  double duration;
#ifdef USE_LOCATION_ID
  size_t locationId;
#endif
  CTraitData::base edgeTrait;
  bool active;
  double weight;
  CNode * pTarget;
  CNode * pSource;
};

#endif /* SRC_NETWORK_CEDGE_H_ */
