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

#ifndef SRC_ACTIONS_CPROGRESSIONACTION_H_
#define SRC_ACTIONS_CPROGRESSIONACTION_H_

#include "actions/CAction.h"
#include "math/CValue.h"

class CProgression;
class CNode;

class CProgressionAction : public CAction
{
public:
  CProgressionAction() = delete;

  CProgressionAction(const CProgressionAction & src) = delete;
  
  CProgressionAction(const CProgression * pProgression, CNode * pTarget);

  virtual ~CProgressionAction();

  virtual double getPriority() const override;
  
  virtual bool execute() const override;

private:
  const CProgression * mpProgression;
  const CNode * mpTarget;
  CValue mStateAtScheduleTime;
};

#endif // SRC_ACTIONS_CPROGRESSIONACTION_H_