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
 * Action.cpp
 *
 *  Created on: Jul 1, 2019
 *      Author: shoops
 */

#include "Action.h"

Action::Action()
  : mPriority(1.0)
  , mCondition()
  , mOperations()
{}

Action::Action(const Action & src)
  : mPriority(src.mPriority)
  , mCondition(src.mCondition)
  , mOperations(src.mOperations)
{}

// virtual
Action::~Action()
{}

double Action::getPriority() const
{
  return mPriority;
}

const Condition & Action::getCondition() const
{
  return mCondition;
}

const std::vector< Operation > & Action::getOperations() const
{
  return mOperations;
}

ActionDefinition::ActionDefinition()
{}

ActionDefinition::ActionDefinition(const ActionDefinition & src)
{}

// virtual
ActionDefinition::~ActionDefinition()
{}

void ActionDefinition::fromJSON(const json_t * json)
{
  // TODO CRITICAL Implement me!
}


