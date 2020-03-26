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

#ifndef SRC_ACTIONS_CTRANSMISSIONACTION_H_
#define SRC_ACTIONS_CTRANSMISSIONACTION_H_

#include "actions/CAction.h"
#include "math/CValue.h"

class CTransmission;
class CNode;

class CTransmissionAction : public CAction
{
public:
  CTransmissionAction() = delete;

  CTransmissionAction(const CTransmissionAction & src) = delete;
  
  CTransmissionAction(const CTransmission * pTransmission, const CNode * pTarget, const CNode * pContact);

  virtual ~CTransmissionAction();

  virtual double getPriority() const override;
  
  virtual bool execute() const override;

private:
  const CTransmission * mpTransmission;
  const CNode * mpTarget;
  CValue mStateAtScheduleTime;
  size_t mContactId;
};

#endif // SRC_ACTIONS_CTRANSMISSIONACTION_H_