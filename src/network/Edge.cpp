// BEGIN: Copyright 
// Copyright (C) 2019 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

/*
 * Edge.cpp
 *
 *  Created on: Jun 26, 2019
 *      Author: shoops
 */

#include "traits/Trait.h"
#include "network/Edge.h"

// static
EdgeData Edge::getDefault()
{
  EdgeData Default;

  Default.targetId = -1;
  Default.targetActivity = Trait::INSTANCES["activityTrait"].getDefault();
  Default.sourceId = -1;
  Default.sourceActivity = Trait::INSTANCES["activityTrait"].getDefault();
  Default.duration = 0.0;
  Default.edgeTrait = Trait::INSTANCES["edgeTrait"].getDefault();
  Default.active = true;
  Default.weight = 1.0;
  Default.pTarget = NULL;
  Default.pSource = NULL;

  return Default;
}

Edge::~Edge()
{}

