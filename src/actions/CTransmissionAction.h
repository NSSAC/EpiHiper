// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2020 - 2023 Rector and Visitors of the University of Virginia 
//  
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal 
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions: 
//  
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software. 
//  
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE 
// END: Copyright 

#ifndef SRC_ACTIONS_CTRANSMISSIONACTION_H_
#define SRC_ACTIONS_CTRANSMISSIONACTION_H_

#include "actions/CAction.h"
#include "math/CValue.h"

class CTransmission;
class CNode;
class CEdge;

class CTransmissionAction : public CAction
{
public:
  CTransmissionAction() = delete;

  CTransmissionAction(const CTransmissionAction & src) = delete;
  
  CTransmissionAction(const CTransmission * pTransmission, const CNode * pTarget, const CEdge * pEdge);

  virtual ~CTransmissionAction();

  virtual size_t getOrder() const override;
  
  virtual bool execute() const override;

private:
  const CTransmission * mpTransmission;
  const CNode * mpTarget;
  CValue mStateAtScheduleTime;
  const CEdge * mpEdge;
};

#endif // SRC_ACTIONS_CTRANSMISSIONACTION_H_