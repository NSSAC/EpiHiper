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

#pragma once

#include "utilities/CLogger.h"

class CEdge;
class CNode;

class CTransmission;
class CHealthState;
class CProgression;

namespace CCustomMethodType 
{
  typedef double (*transmission_propensity)(const CTransmission * pTransmission, const CEdge * pEdge);
  typedef const CProgression * (*state_progression)(const CHealthState * pHealthState, const CNode * pNode);
  typedef unsigned int (*progression_dwell_time)(const CProgression * pProgression, const CNode * pNode);
} // namespace CCustomMethodType

template < class custom_type >
class CCustomMethod
{
public:
  CCustomMethod() = delete;

  CCustomMethod(custom_type pDefaultMethod);

  void setCustomMethod(custom_type pCustomMethod) const;

protected:
  custom_type mpDefaultMethod;
  mutable custom_type mpCustomMethod;
};

template < class custom_type >
CCustomMethod< custom_type >::CCustomMethod(custom_type pDefaultMethod)
  : mpDefaultMethod(pDefaultMethod)
  , mpCustomMethod(pDefaultMethod)
{}

template < class custom_type >
void CCustomMethod< custom_type >::setCustomMethod(custom_type pCustomMethod) const
{
  if (mpCustomMethod != mpDefaultMethod)
    CLogger::warn("CCustomMethod: Overwriting previously set custom method.");

  mpCustomMethod = pCustomMethod;

  if (mpCustomMethod == nullptr)
    mpCustomMethod = mpDefaultMethod;
}
