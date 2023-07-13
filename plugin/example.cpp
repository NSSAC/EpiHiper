// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2022 - 2023 Rector and Visitors of the University of Virginia 
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
      CLogger::info("EpiHiperPlugin: Setting custom method for transmission '{}'.", Transmission.getId());
      Transmission.setCustomMethod(&EpiHiperPlugin::transmission_propensity);
    }

  for (const CHealthState & HealthState : CModel::GetStates())
    {
      CLogger::info("EpiHiperPlugin: Setting custom method for health-state '{}'.", HealthState.getId());
      HealthState.setCustomMethod(&EpiHiperPlugin::state_progression);
    }

  for (const CProgression & Progression : CModel::GetProgressions())
    {
      CLogger::info("EpiHiperPlugin: Setting custom method for progression '{}'.", Progression.getId());
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
          alpha -= pProgression->getPropensity();

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

