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

#ifndef PLUGIN_EXAMPLE_H_
#define PLUGIN_EXAMPLE_H_

extern "C"
{
  void EpiHiperPluginInit();
}

class CTransmission;
class CEdge;
class CHealthState;
class CProgression;
class CNode;

struct EpiHiperPlugin
{
  static double transmission_propensity(const CTransmission * pTransmission, const CEdge * pEdge);
  static const CProgression * state_progression(const CHealthState * pHealthState, const CNode * pNode);
  static unsigned int progression_dwell_time(const CProgression * pProgression, const CNode * pNode);
};

#endif // PLUGIN_EXAMPLE_H_