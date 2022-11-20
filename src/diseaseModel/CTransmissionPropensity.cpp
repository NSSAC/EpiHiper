// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2020 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
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
double CTransmissionPropensity::defaultPropensity(const CEdge * pEdge, const CTransmission * pTransmission)
{
  // ρ(P, P', Τi,j,k) = (| contactTime(P, P') ∩ [tn, tn + Δtn] |) × contactWeight(P, P') × σ(P, Χi) × ι(P',Χk) × ω(Τi,j,k)
  return pEdge->duration * pEdge->weight * pEdge->pTarget->susceptibility
    * pEdge->pSource->infectivity * pTransmission->getTransmissibility();
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
