// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2022 Rector and Visitors of the University of Virginia 
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

#pragma once

#include "utilities/CLogger.h"

class CEdge;
class CNode;

class CTransmission;
class CHealthState;
class CProgression;

namespace CCustomMethodType 
{
  typedef double (*transmission_propensity)(const CTransmission * pTransmission, const CEdge * pEdge);
  typedef const CProgression * (*state_progression)(const CHealthState * pHealthState, const CNode * pNode);
  typedef unsigned int (*progression_dwell_time)(const CProgression * pProgression, const CNode * pNode);
} // namespace CCustomMethodType

template < class custom_type >
class CCustomMethod
{
public:
  CCustomMethod() = delete;

  CCustomMethod(custom_type pDefaultMethod);

  void setCustomMethod(custom_type pCustomMethod) const;

protected:
  custom_type mpDefaultMethod;
  mutable custom_type mpCustomMethod;
};

template < class custom_type >
CCustomMethod< custom_type >::CCustomMethod(custom_type pDefaultMethod)
  : mpDefaultMethod(pDefaultMethod)
  , mpCustomMethod(pDefaultMethod)
{}

template < class custom_type >
void CCustomMethod< custom_type >::setCustomMethod(custom_type pCustomMethod) const
{
  if (mpCustomMethod != mpDefaultMethod)
    CLogger::warn("CCustomMethod: Overwriting previously set custom method.");

  mpCustomMethod = pCustomMethod;

  if (mpCustomMethod == nullptr)
    mpCustomMethod = mpDefaultMethod;
}
