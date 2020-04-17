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

#include "network/CNode.h"
#include "network/CNetwork.h"
#include "diseaseModel/CHealthState.h"
#include "diseaseModel/CModel.h"
#include "diseaseModel/CProgression.h"
#include "diseaseModel/CTransmission.h"
#include "traits/CTrait.h"
#include "utilities/CMetadata.h"
#include "utilities/CLogger.h"

// static
CNode CNode::getDefault()
{
  CNode Default;

  Default.id = -1;
  Default.pHealthState = &CModel::GetInitialState();
  Default.healthState = CModel::StateToType(Default.pHealthState);
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
  , pHealthState(NULL)
  , healthState()
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
  , pHealthState(NULL)
  , healthState()
  , susceptibilityFactor(src.susceptibilityFactor)
  , susceptibility(src.susceptibility)
  , infectivityFactor(src.infectivityFactor)
  , infectivity(src.infectivity)
  , nodeTrait(src.nodeTrait)
  , Edges(src.Edges)
  , EdgesSize(src.EdgesSize)
{
  setHealthState(src.pHealthState);
}

CNode::~CNode()
{}

CNode & CNode::operator = (const CNode & rhs)
{
  if (this != &rhs)
    {
      id = rhs.id;
      setHealthState(rhs.pHealthState);
      susceptibilityFactor = rhs.susceptibilityFactor;
      susceptibility = rhs.susceptibility;
      infectivityFactor = rhs.infectivityFactor;
      infectivity = rhs.infectivity;
      nodeTrait = rhs.nodeTrait;
      Edges = rhs.Edges;
      EdgesSize = rhs.EdgesSize;
    }

  return *this;
}

void CNode::toBinary(std::ostream & os) const
{
  os.write(reinterpret_cast<const char *>(&id), 56);

  /*
  os.write(reinterpret_cast<const char *>(&id), sizeof(size_t));
  os.write(reinterpret_cast<const char *>(&healthState), sizeof( CModel::state_t));
  os.write(reinterpret_cast<const char *>(&susceptibilityFactor), sizeof(double));
  os.write(reinterpret_cast<const char *>(&susceptibility), sizeof(double));
  os.write(reinterpret_cast<const char *>(&infectivityFactor), sizeof(double));
  os.write(reinterpret_cast<const char *>(&infectivity), sizeof(double));
  os.write(reinterpret_cast<const char *>(&nodeTrait), sizeof(CTraitData::base));
  */
}

void CNode::fromBinary(std::istream & is)
{
  is.read(reinterpret_cast<char *>(&id), 56);

  size_t read = is.gcount(); 
  
  if (read != 0
      && read != 56)
    {
      CLogger::error() << "CNode: fromBinary read '" << read << "'."; 
    }

  /*
  is.read(reinterpret_cast<char *>(&id), sizeof(size_t));
  is.read(reinterpret_cast<char *>(&healthState), sizeof(CModel::state_t));
  is.read(reinterpret_cast<char *>(&susceptibilityFactor), sizeof(double));
  is.read(reinterpret_cast<char *>(&susceptibility), sizeof(double));
  is.read(reinterpret_cast<char *>(&infectivityFactor), sizeof(double));
  is.read(reinterpret_cast<char *>(&infectivity), sizeof(double));
  is.read(reinterpret_cast<char *>(&nodeTrait), sizeof(CTraitData::base));
  */

  pHealthState = CModel::StateFromType(healthState);
}

bool CNode::set(const CTransmission * pTransmission, CValueInterface::pOperator /* pOperator */, const CMetadata & metadata)
{
  if (pHealthState == pTransmission->getExitState()) return false;

  CLogger::trace() << "CNode [Transmission]: Node ("
                   << id
                   << ") healthState = "
                   << pTransmission->getExitState()->getId();
                   
  setHealthState(pTransmission->getExitState());

  pTransmission->updateSusceptibilityFactor(susceptibilityFactor);
  susceptibility = pHealthState->getSusceptibility() * susceptibilityFactor;
  pTransmission->updateInfectivityFactor(infectivityFactor);
  infectivity = pHealthState->getInfectivity() * infectivity;

  // std::cout << id << "," << pTransmission->getEntryState() << "," << pTransmission->getExitState() << "," << pTransmission->getContactState() << std::endl;

  CModel::StateChanged(this);

  return true;
}

bool CNode::set(const CProgression * pProgression, CValueInterface::pOperator /* pOperator */, const CMetadata & metadata)
{
  if (pHealthState == pProgression->getExitState()) return false;

  CLogger::trace() << "CNode [Progression]: Node ("
                   << id
                   << ") healthState = "
                   << pProgression->getExitState()->getId();
  
  setHealthState(pProgression->getExitState());

  pProgression->updateSusceptibilityFactor(susceptibilityFactor);
  susceptibility = pHealthState->getSusceptibility() * susceptibilityFactor;
  pProgression->updateInfectivityFactor(infectivityFactor);
  infectivity = pHealthState->getInfectivity() * infectivityFactor;

  // std::cout << id << "," << pProgression->getEntryState() << "," << pProgression->getExitState() << std::endl;

  CModel::StateChanged(this);

  return true;
}

bool CNode::setSusceptibilityFactor(double value, CValueInterface::pOperator pOperator, const CMetadata & metadata)
{
  CLogger::trace() << "CNode [ActionDefinition:"
                   << (metadata.contains("CActionDefinition") ? metadata.getInt("CActionDefinition") : -1)
                   << "]: Node ("
                   << id
                   << ") susceptibilityFactor "
                   << CValueInterface::operatorToString(pOperator)
                   << " "
                   << value;
  (*pOperator)(susceptibilityFactor, value);
  susceptibility = pHealthState->getSusceptibility() * susceptibilityFactor;

  return true;
}

bool CNode::setInfectivityFactor(double value, CValueInterface::pOperator pOperator, const CMetadata & metadata)
{
  CLogger::trace() << "CNode [ActionDefinition:"
                   << (metadata.contains("CActionDefinition") ? metadata.getInt("CActionDefinition") : -1)
                   << "]: Node ("
                   << id
                   << ") infectivityFactor "
                   << CValueInterface::operatorToString(pOperator)
                   << " "
                   << value;
  (*pOperator)(infectivityFactor, value);
  infectivity = pHealthState->getInfectivity() * infectivityFactor;

  return true;
}

bool CNode::setHealthState(CModel::state_t value, CValueInterface::pOperator pOperator, const CMetadata & metadata)
{
  CLogger::trace() << "CNode [ActionDefinition:"
                   << (metadata.contains("CActionDefinition") ? metadata.getInt("CActionDefinition") : -1)
                   << "]: Node ("
                   << id
                   << ") healthState "
                   << CValueInterface::operatorToString(pOperator)
                   << " "
                   << CModel::StateFromType(value)->getId();
  setHealthState(CModel::StateFromType(value));

  susceptibility = pHealthState->getSusceptibility() * susceptibilityFactor;
  infectivity = pHealthState->getInfectivity() * infectivityFactor;

  CModel::StateChanged(this);

  return true;
}

bool CNode::setNodeTrait(CTraitData::value value, CValueInterface::pOperator pOperator, const CMetadata & metadata)
{
  CLogger::trace() << "CNode [ActionDefinition:"
                   << (metadata.contains("CActionDefinition") ? metadata.getInt("CActionDefinition") : -1)
                   << "]: Node ("
                   << id
                   << ") nodeTrait "
                   << CValueInterface::operatorToString(pOperator)
                   << " "
                   << CTrait::NodeTrait->toString(value);
  CTraitData::setValue(nodeTrait, value);

  return true;
}

void CNode::setHealthState(const CHealthState * pNewHealthState)
{
  if (CNetwork::INSTANCE->isRemoteNode(this))
    {
      pHealthState = pNewHealthState;
      healthState = CModel::StateToType(pHealthState);
      return;
    }

  if (pHealthState != NULL)
    pHealthState->decrement();

  pHealthState = pNewHealthState;

  if (pHealthState != NULL)
    pHealthState->increment();

  healthState = CModel::StateToType(pHealthState);
}


