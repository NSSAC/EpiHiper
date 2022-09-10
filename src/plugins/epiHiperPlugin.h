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

class CEdge;
class CNode;

class CTransmission;
class CHealthState;
class CProgression;

namespace epiHiperPlugin 
{
  typedef double (*transmission_propensity)(const CTransmission * pTransmission, const CEdge * pEdge);
  typedef const CProgression * (*state_progression)(const CHealthState * pHealthState, const CNode * pNode);
  typedef unsigned int (*progression_dwell_time)(const CProgression * pProgression, const CNode * pNode);
} // namespace epiHiperPlugin

template < class custom_type >
class CCustomMethod
{
public:
  void setCustomMethod(custom_type pCustomMethod);

protected:
  custom_type mpCustomMethod = nullptr;
};

template < class custom_type >
void CCustomMethod< custom_type >::setCustomMethod(custom_type pCustomMethod)
{
  mpCustomMethod = pCustomMethod;
}
