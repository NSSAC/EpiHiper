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

#include "dwellTime.h"

#include "diseaseModel/CProgression.h"
#include "network/CNode.h"
#include "network/CEdge.h"

unsigned int dwellTime(const CProgression * pProgression, const CNode * /* pNode */)
{
  return pProgression->getDwellTime();
}