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

#ifndef SRC_ACTIONS_CEDGEACTION_H_
#define SRC_ACTIONS_CEDGEACTION_H_

#include "actions/CAction.h"

class CActionDefinition;
class CEdge;

class CEdgeAction : public CAction
{
public:
  CEdgeAction() = delete;

  CEdgeAction(const CEdgeAction & src) = delete;
  
  CEdgeAction(const CActionDefinition * pDefinition, const CEdge * pTarget);

  virtual ~CEdgeAction();

  virtual double getPriority() const override;
  
  virtual bool execute() const override;

private:
  const CActionDefinition * mpDefinition;
  const CEdge * mpTarget;  
};

#endif // SRC_ACTIONS_CEDGEACTION_H_