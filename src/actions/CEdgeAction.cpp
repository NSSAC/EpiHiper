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

#include "actions/CEdgeAction.h"
#include "actions/CActionDefinition.h"

CEdgeAction::CEdgeAction(const CActionDefinition * pDefinition, const CEdge * pTarget)
  : CAction()
  , mpDefinition(pDefinition)
  , mpTarget(pTarget)
{}

// virtual 
CEdgeAction::~CEdgeAction()
{}

// virtual 
double CEdgeAction::getPriority() const
{
  return mpDefinition->getPriority();
}

// virtual 
bool CEdgeAction::execute() const
{
  return mpDefinition->execute(const_cast< CEdge * >(mpTarget));
}
