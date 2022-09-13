// BEGIN: Copyright 
// Copyright (C) 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include "example.h"

#include "diseaseModel/CModel.h"
#include "diseaseModel/CHealthState.h"
#include "diseaseModel/CTransmission.h"
#include "diseaseModel/CProgression.h"
#include "network/CNode.h"
#include "network/CEdge.h"
#include "utilities/CLogger.h"

void EpiHiperPluginInit()
{
  for (const CTransmission & Transmission : CModel::GetTransmissions())
    {
      CLogger::info() << "EpiHiperPlugin: Setting custom method for transmission '" << Transmission.getId() << "'.";
      Transmission.setCustomMethod(&EpiHiperPlugin::transmission_propensity);
    }

  for (const CHealthState & HealthState : CModel::GetStates())
    {
      CLogger::info() << "EpiHiperPlugin: Setting custom method for health-state '" << HealthState.getId() << "'.";
      HealthState.setCustomMethod(&EpiHiperPlugin::state_progression);
    }

  for (const CProgression & Progression : CModel::GetProgressions())
    {
      CLogger::info() << "EpiHiperPlugin: Setting custom method for progression '" << Progression.getId() << "'.";
      Progression.setCustomMethod(&EpiHiperPlugin::progression_dwell_time);
    }
}

// static 
double EpiHiperPlugin::transmission_propensity(const CTransmission * pTransmission, const CEdge * pEdge)
{
  // ρ(P, P', Τi,j,k) = (| contactTime(P, P') ∩ [tn, tn + Δtn] |) × contactWeight(P, P') × σ(P, Χi) × ι(P',Χk) × ω(Τi,j,k)
  return pEdge->duration * pEdge->weight * pEdge->pTarget->susceptibility
         * pEdge->pSource->infectivity * pTransmission->getTransmissibility();
}

// static 
const CProgression * EpiHiperPlugin::state_progression(const CHealthState * pHealthState, const CNode * pNode)
{
  const CHealthState::PossibleProgressions & Progressions = pHealthState->getPossibleProgressions();

  if (Progressions.A0 > 0.0)
    {
      double alpha = CRandom::uniform_real(0.0, Progressions.A0)(CRandom::G.Active());

      for (const CProgression * pProgression : Progressions.Progressions)
        {
          alpha -= pProgression->getProbability();

          if (alpha < 0.0)
            return pProgression;
        }

      return Progressions.Progressions.back();
    }

  return NULL;
}

// static 
unsigned int EpiHiperPlugin::progression_dwell_time(const CProgression * pProgression, const CNode * pNode)
{
  return CProgression::defaultMethod(pProgression, pNode);
}

