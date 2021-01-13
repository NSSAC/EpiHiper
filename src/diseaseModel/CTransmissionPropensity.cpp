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

#include <stdio.h>
#include <dlfcn.h>

#include "diseaseModel/CTransmissionPropensity.h"

#include "diseaseModel/CTransmission.h"
#include "network/CEdge.h"
#include "network/CNode.h"
#include "utilities/CLogger.h"
#include "utilities/CSimConfig.h"
void * lib_handle = NULL;

// static 
double CTransmissionPropensity::defaultPropensity(const CEdge * pEdge, const CTransmission * pTransmission, const double & transmissability)
{
  // ρ(P, P', Τi,j,k) = (| contactTime(P, P') ∩ [tn, tn + Δtn] |) × contactWeight(P, P') × σ(P, Χi) × ι(P',Χk) × ω(Τi,j,k)
  return pEdge->duration * pEdge->weight * pEdge->pTarget->susceptibility
    * pEdge->pSource->infectivity * pTransmission->getTransmissibility() * transmissability;
}

// static 
void CTransmissionPropensity::Init()
{
  std::string PropensityPlugin = CSimConfig::getPropensityPlugin();

  if (PropensityPlugin.empty())
    return;

  lib_handle = dlopen(PropensityPlugin.c_str(), RTLD_NOW | RTLD_DEEPBIND);

  if (lib_handle == NULL)
    {
      CLogger::error() << "CTransmissionPropensity: " << dlerror();
    }

   char *error;
   CTransmissionPropensity::pPROPENSITY = (pPropensity) dlsym(lib_handle, "propensity");

   if ((error = dlerror()) != NULL) 
    {
      CLogger::error() << "CTransmissionPropensity: " << error;
    }
}

// static 
void CTransmissionPropensity::Release()
{
  if (lib_handle != NULL)
    dlclose(lib_handle);
}

// static 
CTransmissionPropensity::pPropensity CTransmissionPropensity::pPROPENSITY = &CTransmissionPropensity::defaultPropensity;
