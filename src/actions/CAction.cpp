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

#include "actions/CAction.h"

CAction::CAction(const double & priority,
               const CCondition & condition,
               const CMetadata & metadata)
  : mPriority(priority)
  , mCondition(condition)
  , mOperations()
  , mMetadata(metadata)
{}

CAction::CAction(const CAction & src)
  : mPriority(src.mPriority)
  , mCondition(src.mCondition)
  , mOperations(src.mOperations.size())
  , mMetadata(src.mMetadata)
{
  std::vector< COperation * >::const_iterator itSrc = src.mOperations.begin();
  std::vector< COperation * >::iterator it = mOperations.begin();
  std::vector< COperation * >::iterator end = mOperations.end();

  for (; it != end; ++it, ++itSrc)
    {
      *it = (*itSrc)->copy();
    }
}

// virtual
CAction::~CAction()
{
  std::vector< COperation * >::iterator it = mOperations.begin();
  std::vector< COperation * >::iterator end = mOperations.end();

  for (; it != end; ++it)
    if (*it != NULL)
      delete *it;
}

double CAction::getPriority() const
{
  return mPriority;
}

const CCondition & CAction::getCondition() const
{
  return mCondition;
}

const std::vector< COperation * > & CAction::getOperations() const
{
  return mOperations;
}

void CAction::addOperation(COperation * pOperation)
{
  mOperations.push_back(pOperation);
}

/*
void CAction::addOperation(const COperation & operation)
{
  // TODO CRITICAL We need to deal with operations on remote objects.

  mOperations.push_back(operation.copy());
}
*/

const CMetadata & CAction::getMetadata() const
{
  return mMetadata;
}

void CAction::toBinary(std::ostream & os) const
{
  //TODO CRITICAL Implement me!
}

bool CAction::fromBinary(std::istream & is)
{
  bool success = true;

  //TODO CRITICAL Implement me!

  return success;
}


CActionDefinition::CActionDefinition()
{}

CActionDefinition::CActionDefinition(const CActionDefinition & src)
{}

// virtual
CActionDefinition::~CActionDefinition()
{}

void CActionDefinition::fromJSON(const json_t * json)
{
  // TODO CRITICAL Implement me!
}


