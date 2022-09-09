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

#include "plugins/CDwellTime.h"

#include "plugins/CPlugin.h"
#include "diseaseModel/CProgression.h"
#include "network/CNode.h"
#include "utilities/CSimConfig.h"

// static
unsigned int CDwellTime::defaultCalculate(const CProgression * pProgression, const CNode * /* pNode */)
{
  return pProgression->getDwellTime();
}

// static
CDwellTime::calculate_type CDwellTime::calculate = &CDwellTime::defaultCalculate;

// static
void CDwellTime::Init()
{
  CPlugin Plugin(CSimConfig::getPlugin(CSimConfig::Plugin::dwellTime));

  CDwellTime::calculate = Plugin.symbol< calculate_type >("dwellTime");

  if (CDwellTime::calculate == nullptr)
    {
      CDwellTime::calculate = &CDwellTime::defaultCalculate;
    }
}
