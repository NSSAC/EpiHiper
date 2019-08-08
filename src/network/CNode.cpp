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

#include "network/CNode.h"
#include "diseaseModel/CHealthState.h"
#include "diseaseModel/CModel.h"
#include "diseaseModel/CProgression.h"
#include "diseaseModel/CTransmission.h"
#include "traits/CTrait.h"


// static
CNode CNode::getDefault()
{
  CNode Default;

  Default.id = -1;
  Default.pHealthState = &CModel::getInitialState();
  Default.susceptibilityFactor = 1.0;
  Default.susceptibility = Default.pHealthState->getSusceptibility();
  Default.infectivityFactor = 1.0;
  Default.infectivity = Default.pHealthState->getInfectivity();
  Default.nodeTrait = CTrait::NodeTrait->getDefault();
  Default.Edges = NULL;
  Default.EdgesSize = 0;

  return Default;
}

CNode::CNode()
  : id(-1)
  , pHealthState()
  , susceptibilityFactor(1.0)
  , susceptibility(0.0)
  , infectivityFactor(1.0)
  , infectivity(0.0)
  , nodeTrait()
  , Edges(NULL)
  , EdgesSize(0)
{}

CNode::CNode(const CNode & src)
  : id(src.id)
  , pHealthState(src.pHealthState)
  , susceptibilityFactor(src.susceptibilityFactor)
  , susceptibility(src.susceptibility)
  , infectivityFactor(src.infectivityFactor)
  , infectivity(src.infectivity)
  , nodeTrait(src.nodeTrait)
  , Edges(src.Edges)
  , EdgesSize(src.EdgesSize)
{}

CNode::~CNode()
{}

void CNode::toBinary(std::ostream & os) const
{
  CModel::state_t Type = CModel::stateToType(pHealthState);

  os.write(reinterpret_cast<const char *>(&id), sizeof(size_t));
  os.write(reinterpret_cast<const char *>(&Type), sizeof( CModel::state_t));
  os.write(reinterpret_cast<const char *>(&susceptibilityFactor), sizeof(double));
  os.write(reinterpret_cast<const char *>(&susceptibility), sizeof(double));
  os.write(reinterpret_cast<const char *>(&infectivityFactor), sizeof(double));
  os.write(reinterpret_cast<const char *>(&infectivity), sizeof(double));
  os.write(reinterpret_cast<const char *>(&nodeTrait), sizeof(CTraitData::base));
}

void CNode::fromBinary(std::istream & is)
{
  CModel::state_t Type;

  is.read(reinterpret_cast<char *>(&id), sizeof(size_t));
  is.read(reinterpret_cast<char *>(&Type), sizeof(CModel::state_t));
  is.read(reinterpret_cast<char *>(&susceptibilityFactor), sizeof(double));
  is.read(reinterpret_cast<char *>(&susceptibility), sizeof(double));
  is.read(reinterpret_cast<char *>(&infectivityFactor), sizeof(double));
  is.read(reinterpret_cast<char *>(&infectivity), sizeof(double));
  is.read(reinterpret_cast<char *>(&nodeTrait), sizeof(CTraitData::base));

  pHealthState = CModel::stateFromType(Type);
}

bool CNode::set(const CTransmission * pTransmission, const CMetadata & metadata)
{
  if (pHealthState == pTransmission->getExitState()) return false;

  pHealthState = pTransmission->getExitState();
  pTransmission->updateSusceptibilityFactor(susceptibilityFactor);
  susceptibility = pHealthState->getSusceptibility() * susceptibilityFactor;
  pTransmission->updateInfectivityFactor(infectivityFactor);
  infectivity = pHealthState->getInfectivity() * infectivity;

  // std::cout << id << "," << pTransmission->getEntryState() << "," << pTransmission->getExitState() << "," << pTransmission->getContactState() << std::endl;

  CModel::stateChanged(this);

  return true;
}

bool CNode::set(const CProgression * pProgression, const CMetadata & metadata)
{
  if (pHealthState == pProgression->getExitState()) return false;

  pHealthState = pProgression->getExitState();
  pProgression->updateSusceptibilityFactor(susceptibilityFactor);
  susceptibility = pHealthState->getSusceptibility() * susceptibilityFactor;
  pProgression->updateInfectivityFactor(infectivityFactor);
  infectivity = pHealthState->getInfectivity() * infectivityFactor;

  // std::cout << id << "," << pProgression->getEntryState() << "," << pProgression->getExitState() << std::endl;

  CModel::stateChanged(this);

  return true;
}

bool CNode::setSusceptibilityFactor(double value, const CMetadata & metadata)
{
  susceptibilityFactor = value;
  susceptibility = pHealthState->getSusceptibility() * susceptibilityFactor;

  return true;
}

bool CNode::setInfectivityFactor(double value, const CMetadata & metadata)
{
  infectivityFactor = value;
  infectivity = pHealthState->getInfectivity() * infectivityFactor;

  return true;
}

bool CNode::setHealthState(CModel::state_t value, const CMetadata & metadata)
{
  pHealthState = CModel::stateFromType(value);
  susceptibility = pHealthState->getSusceptibility() * susceptibilityFactor;
  infectivity = pHealthState->getInfectivity() * infectivityFactor;

  CModel::stateChanged(this);

  return true;
}

bool CNode::setNodeTrait(CTraitData::value value, const CMetadata & metadata)
{
  CTraitData::setValue(nodeTrait, value);

  return true;
}


