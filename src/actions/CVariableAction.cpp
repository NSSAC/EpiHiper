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

#include "actions/CVariableAction.h"
#include "actions/CActionDefinition.h"

CVariableAction::CVariableAction(const CActionDefinition * pDefinition)
  : CAction()
  , mpDefinition(pDefinition){}

// virtual 
CVariableAction::~CVariableAction()
{}

// virtual 
double CVariableAction::getPriority() const
{
  return mpDefinition->getPriority();
}

// virtual 
bool CVariableAction::execute() const
{
  return mpDefinition->execute();
}
