// BEGIN: Copyright 
// Copyright (C) 2020 - 2021 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include "example.h"

#include "diseaseModel/CTransmission.h"
#include "network/CNode.h"
#include "network/CEdge.h"

double propensity(const CEdge * pEdge, const CTransmission * pTransmission, const double & transmissability)
{
  return pEdge->duration * pEdge->weight * pEdge->pTarget->susceptibility
    * pEdge->pSource->infectivity * pTransmission->getTransmissibility() * transmissability;
}