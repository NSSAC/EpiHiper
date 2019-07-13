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

Action::Action(const double & priority,
               const Condition & condition,
               const Metadata & metadata)
  : mPriority(priority)
  , mCondition(condition)
  , mOperations()
  , mMetadata(metadata)
{}

Action::Action(const Action & src)
  : mPriority(src.mPriority)
  , mCondition(src.mCondition)
  , mOperations(src.mOperations.size())
  , mMetadata(src.mMetadata)
{
  std::vector< Operation * >::const_iterator itSrc = src.mOperations.begin();
  std::vector< Operation * >::iterator it = mOperations.begin();
  std::vector< Operation * >::iterator end = mOperations.end();

  for (; it != end; ++it, ++itSrc)
    {
      *it = (*itSrc)->copy();
    }
}

// virtual
Action::~Action()
{
  std::vector< Operation * >::iterator it = mOperations.begin();
  std::vector< Operation * >::iterator end = mOperations.end();

  for (; it != end; ++it)
    if (*it != NULL)
      delete *it;
}

double Action::getPriority() const
{
  return mPriority;
}

const Condition & Action::getCondition() const
{
  return mCondition;
}

const std::vector< Operation * > & Action::getOperations() const
{
  return mOperations;
}

void Action::addOperation(const Operation & operation)
{
  // TODO CRITICAL We need to deal with operations on remote objects.

  mOperations.push_back(operation.copy());
}

const Metadata & Action::getMetadata() const
{
  return mMetadata;
}

void Action::toBinary(std::ostream & os) const
{
  //TODO CRITICAL Implement me!
}

bool Action::fromBinary(std::istream & is)
{
  bool success = true;

  //TODO CRITICAL Implement me!

  return success;
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


