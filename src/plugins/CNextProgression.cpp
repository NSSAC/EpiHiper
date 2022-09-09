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

#include <stdio.h>
#include <dlfcn.h>

#include "plugins/CNextProgression.h"

#include "plugins/CPlugin.h"
#include "diseaseModel/CHealthState.h"
#include "network/CNode.h"
#include "utilities/CSimConfig.h"

// static
const CProgression * CNextProgression::defaultCalculate(const CHealthState * pHealthState, const CNode * /* pNode */)
{
  return pHealthState->nextProgression();
}

// static
CNextProgression::calculate_type CNextProgression::calculate = &CNextProgression::defaultCalculate;

// static
void CNextProgression::Init()
{
  CPlugin Plugin(CSimConfig::getPlugin(CSimConfig::Plugin::nextProgression));

  CNextProgression::calculate = Plugin.symbol< calculate_type >("nextProgression");

  if (CNextProgression::calculate == nullptr)
    {
      CNextProgression::calculate = &CNextProgression::defaultCalculate;
    }
}
