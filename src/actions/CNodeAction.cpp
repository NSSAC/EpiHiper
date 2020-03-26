// BEGIN: Copyright 
// Copyright (C) 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include "actions/CNodeAction.h"
#include "actions/CActionDefinition.h"

CNodeAction::CNodeAction(const CActionDefinition * pDefinition, const CNode * pTarget)
  : CAction()
  , mpDefinition(pDefinition)
  , mpTarget(pTarget)
{}

// virtual 
CNodeAction::~CNodeAction()
{}

// virtual 
double CNodeAction::getPriority() const
{
  return mpDefinition->getPriority();
}

// virtual 
bool CNodeAction::execute() const
{
  return mpDefinition->execute(const_cast< CNode * >(mpTarget));
}
