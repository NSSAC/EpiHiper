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

#ifndef SRC_ACTIONS_CVARIABLEACTION_H_
#define SRC_ACTIONS_CVARIABLEACTION_H_

#include "actions/CAction.h"

class CActionDefinition;

class CVariableAction : public CAction
{
public:
  CVariableAction() = delete;

  CVariableAction(const CVariableAction & src) = delete;
  
  CVariableAction(const CActionDefinition * pDefinition);

  virtual ~CVariableAction();

  virtual double getPriority() const override;
  
  virtual bool execute() const override;

private:
  const CActionDefinition * mpDefinition;
};

#endif // SRC_ACTIONS_CVARIABLEACTION_H_