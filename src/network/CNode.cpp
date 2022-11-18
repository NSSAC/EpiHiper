// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
//  
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal 
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions: 
//  
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software. 
//  
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE 
// END: Copyright 

#include "network/CNode.h"
#include "network/CEdge.h"
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

  Default.pHealthState = &CModel::GetInitialState();
  Default.healthState = Default.pHealthState->getIndex();
  Default.susceptibility = Default.pHealthState->getSusceptibility();
  Default.infectivity = Default.pHealthState->getInfectivity();
  Default.nodeTrait = CTrait::NodeTrait->getDefault();

  return Default;
}

CNode::CNode()
  : id(-1)
  , healthState()
  , susceptibilityFactor(1.0)
  , susceptibility(0.0)
  , infectivityFactor(1.0)
  , infectivity(0.0)
  , nodeTrait()
  , Edges(NULL)
  , EdgesSize(0)
  , OutgoingEdges()
  , pHealthState(NULL)
{
  OutgoingEdges.init();
  
  OutgoingEdges.Master().pEdges = NULL;
  OutgoingEdges.Master().Size = 0;

  sOutgoingEdges * pIt = OutgoingEdges.beginThread();
  sOutgoingEdges * pEnd = OutgoingEdges.endThread();

  for (; pIt != pEnd; ++pIt)
    if (OutgoingEdges.isThread(pIt))
      {
        pIt->pEdges = NULL;
        pIt->Size = 0;
      }
}

CNode::CNode(const CNode & src)
  : id(src.id)
  , healthState()
  , susceptibilityFactor(src.susceptibilityFactor)
  , susceptibility(src.susceptibility)
  , infectivityFactor(src.infectivityFactor)
  , infectivity(src.infectivity)
  , nodeTrait(src.nodeTrait)
  , Edges(src.Edges)
  , EdgesSize(src.EdgesSize)
  , OutgoingEdges()
  , pHealthState(NULL)
{
  OutgoingEdges.init();
  
  OutgoingEdges.Master().pEdges = NULL;
  OutgoingEdges.Master().Size = 0;

  sOutgoingEdges * pIt = OutgoingEdges.beginThread();
  sOutgoingEdges * pEnd = OutgoingEdges.endThread();

  for (; pIt != pEnd; ++pIt)
    if (OutgoingEdges.isThread(pIt))
      {
        pIt->pEdges = NULL;
        pIt->Size = 0;
      }

  assert(OutgoingEdges == src.OutgoingEdges);
 
  setHealthState(src.pHealthState);
}

CNode::~CNode()
{
  if (OutgoingEdges.Master().pEdges != NULL)
    {
      delete [] OutgoingEdges.Master().pEdges;
    }

  sOutgoingEdges * pIt = OutgoingEdges.beginThread();
  sOutgoingEdges * pEnd = OutgoingEdges.endThread();

  for (; pIt != pEnd; ++pIt)
    if (OutgoingEdges.isThread(pIt)  
        && pIt->pEdges != NULL)
      delete [] pIt->pEdges;
}

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

      assert(OutgoingEdges == rhs.OutgoingEdges);
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

  {
    CLogger::trace Trace;

    Trace << "CNode [Transmission]: Node ("
          << id
          << ") healthState = "
          << pTransmission->getExitState()->getId()
          << ", contact: "
          << (size_t) metadata.getInt("ContactNode");

    if (CEdge::HasLocationId)
      {
        Trace << ", location: "
          << (size_t) metadata.getInt("LocationId");
      }
  }
                   
  setHealthState(pTransmission->getExitState());

  pTransmission->updateSusceptibilityFactor(susceptibilityFactor);
  susceptibility = pHealthState->getSusceptibility() * susceptibilityFactor;
  pTransmission->updateInfectivityFactor(infectivityFactor);
  infectivity = pHealthState->getInfectivity() * infectivityFactor;

  // std::cout << id << "," << pTransmission->getEntryState() << "," << pTransmission->getExitState() << "," << pTransmission->getContactState() << std::endl;

  CModel::StateChanged(this);

  return true;
}

bool CNode::set(const CProgression * pProgression, CValueInterface::pOperator /* pOperator */, const CMetadata & /* metadata */)
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
  if (CNetwork::Context.Active().isRemoteNode(this))
    {
      pHealthState = pNewHealthState;
      healthState = pHealthState->getIndex();
      return;
    }

  if (pHealthState != NULL)
    pHealthState->decrement();

  pHealthState = pNewHealthState;

  if (pHealthState != NULL)
    pHealthState->increment();

  healthState = pHealthState->getIndex();
}


