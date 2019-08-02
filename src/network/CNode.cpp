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

#include "CNode.h"

#include "diseaseModel/Model.h"
#include "diseaseModel/State.h"
#include "diseaseModel/Transmission.h"
#include "diseaseModel/Progression.h"
#include "traits/Trait.h"


// static
CNode CNode::getDefault()
{
  CNode Default;

  Default.id = -1;
  Default.pHealthState = &Model::getInitialState();
  Default.susceptibilityFactor = 1.0;
  Default.susceptibility = Default.pHealthState->getSusceptibility();
  Default.infectivityFactor = 1.0;
  Default.infectivity = Default.pHealthState->getInfectivity();
  Default.nodeTrait = Trait::NodeTrait->getDefault();
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
  Model::state_t Type = Model::stateToType(pHealthState);

  os.write(reinterpret_cast<const char *>(&id), sizeof(size_t));
  os.write(reinterpret_cast<const char *>(&Type), sizeof( Model::state_t));
  os.write(reinterpret_cast<const char *>(&susceptibilityFactor), sizeof(double));
  os.write(reinterpret_cast<const char *>(&susceptibility), sizeof(double));
  os.write(reinterpret_cast<const char *>(&infectivityFactor), sizeof(double));
  os.write(reinterpret_cast<const char *>(&infectivity), sizeof(double));
  os.write(reinterpret_cast<const char *>(&nodeTrait), sizeof(TraitData::base));
}

void CNode::fromBinary(std::istream & is)
{
  Model::state_t Type;

  is.read(reinterpret_cast<char *>(&id), sizeof(size_t));
  is.read(reinterpret_cast<char *>(&Type), sizeof(Model::state_t));
  is.read(reinterpret_cast<char *>(&susceptibilityFactor), sizeof(double));
  is.read(reinterpret_cast<char *>(&susceptibility), sizeof(double));
  is.read(reinterpret_cast<char *>(&infectivityFactor), sizeof(double));
  is.read(reinterpret_cast<char *>(&infectivity), sizeof(double));
  is.read(reinterpret_cast<char *>(&nodeTrait), sizeof(TraitData::base));

  pHealthState = Model::stateFromType(Type);
}

bool CNode::set(const Transmission * pTransmission, const Metadata & metadata)
{
  if (pHealthState == pTransmission->getExitState()) return false;

  pHealthState = pTransmission->getExitState();
  pTransmission->updateSusceptibilityFactor(susceptibilityFactor);
  susceptibility = pHealthState->getSusceptibility() * susceptibilityFactor;
  pTransmission->updateInfectivityFactor(susceptibilityFactor);
  infectivity = pHealthState->getInfectivity() * infectivity;

  // std::cout << id << "," << pTransmission->getEntryState() << "," << pTransmission->getExitState() << "," << pTransmission->getContactState() << std::endl;

  Model::stateChanged(this);

  return true;
}

bool CNode::set(const Progression * pProgression, const Metadata & metadata)
{
  if (pHealthState == pProgression->getExitState()) return false;

  pHealthState = pProgression->getExitState();
  pProgression->updateSusceptibilityFactor(susceptibilityFactor);
  susceptibility = pHealthState->getSusceptibility() * susceptibilityFactor;
  pProgression->updateInfectivityFactor(susceptibilityFactor);
  infectivity = pHealthState->getInfectivity() * infectivityFactor;

  // std::cout << id << "," << pProgression->getEntryState() << "," << pProgression->getExitState() << std::endl;

  Model::stateChanged(this);

  return true;
}


