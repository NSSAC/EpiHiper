// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
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
                CCondition * pCondition)
  : mPriority(priority)
  , mpCondition(pCondition)
  , mOperations()
{}

// virtual
CAction::~CAction()
{
  delete mpCondition; 
  
  std::vector< COperation * >::iterator it = mOperations.begin();
  std::vector< COperation * >::iterator end = mOperations.end();

  for (; it != end; ++it)
    delete *it;
}

bool CAction::execute() const
{
  bool success = true;

  if (mpCondition->isTrue())
    {
      std::vector< COperation * >::const_iterator it = mOperations.begin();
      std::vector< COperation * >::const_iterator end = mOperations.end();

      for (; it != end; ++it)
        success &= (*it)->execute();
    }

  return success;
}
  

double CAction::getPriority() const
{
  return mPriority;
}

void CAction::addOperation(COperation * pOperation)
{
  if (pOperation != NULL)
    mOperations.push_back(pOperation);
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

