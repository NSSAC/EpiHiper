// BEGIN: Copyright 
// Copyright (C) 2020 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include <stdio.h>
#include <dlfcn.h>

#include "diseaseModel/CTransmissionPropensity.h"

#include "diseaseModel/CTransmission.h"
#include "network/CEdge.h"
#include "network/CNode.h"
#include "utilities/CPlugin.h"
#include "utilities/CSimConfig.h"

// static
double CTransmissionPropensity::defaultPropensity(const CEdge * pEdge, const CTransmission * pTransmission)
{
  // ρ(P, P', Τi,j,k) = (| contactTime(P, P') ∩ [tn, tn + Δtn] |) × contactWeight(P, P') × σ(P, Χi) × ι(P',Χk) × ω(Τi,j,k)
  return pEdge->duration * pEdge->weight * pEdge->pTarget->susceptibility
         * pEdge->pSource->infectivity * pTransmission->getTransmissibility();
}

// static
CTransmissionPropensity::calculate_type CTransmissionPropensity::calculate = &CTransmissionPropensity::defaultPropensity;

// static
void CTransmissionPropensity::Init()
{
  CPlugin Plugin(CSimConfig::getPropensityPlugin());

  CTransmissionPropensity::calculate = Plugin.symbol< calculate_type > ("propensity");

  if (CTransmissionPropensity::calculate == nullptr)
    {
      CTransmissionPropensity::calculate = &CTransmissionPropensity::defaultPropensity;
    }
}
