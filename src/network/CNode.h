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

/*
 * Node.h
 *
 *  Created on: Jun 26, 2019
 *      Author: shoops
 */

#ifndef SRC_NETWORK_CNODE_H_
#define SRC_NETWORK_CNODE_H_

#include <iostream>

#include "traits/CTraitData.h"
#include "diseaseModel/CModel.h"

class CHealthState;
class CTransmission;
class CProgression;
class CMetadata;

class CEdge;

class CNode
{
public:
  static CNode getDefault();

  CNode();
  CNode(const CNode & src);

  virtual ~CNode();

  void toBinary(std::ostream & os) const;
  void fromBinary(std::istream & is);

  bool set(const CTransmission * pTransmission, const CMetadata & metadata);
  bool set(const CProgression * pProgression, const CMetadata & metadata);
  bool setSusceptibilityFactor(double value, const CMetadata & metadata);
  bool setInfectivityFactor(double value, const CMetadata & metadata);
  bool setHealthState(CModel::state_t value, const CMetadata & metadata);
  bool setNodeTrait(CTraitData::value value, const CMetadata & metadata);

  size_t id;
  const CHealthState * pHealthState;
  double susceptibilityFactor;
  double susceptibility;
  double infectivityFactor;
  double infectivity;
  CTraitData::base nodeTrait;
  CEdge * Edges;
  size_t EdgesSize;
};

#endif /* SRC_NETWORK_CNODE_H_ */
