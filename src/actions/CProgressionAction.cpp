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

#include "actions/CProgressionAction.h"
#include "actions/COperation.h"
#include "diseaseModel/CProgression.h"
#include "diseaseModel/CHealthState.h"
#include "network/CNode.h"
#include "utilities/CMetadata.h"
#include "utilities/CLogger.h"
#include "math/CNodeProperty.h"

CProgressionAction::CProgressionAction(const CProgression * pProgression, CNode * pTarget)
  : CAction()
  , mpProgression(pProgression)
  , mpTarget(pTarget)
  , mStateAtScheduleTime(pTarget->healthState)
{
  ENABLE_TRACE(CLogger::trace("CProgressionAction: Add action Node '{}' healthState = '{}'.", mpTarget->id, mpProgression->getExitState()->getId()););
}

// virtual
CProgressionAction::~CProgressionAction()
{}

// virtual
size_t CProgressionAction::getOrder() const
{
  return DefaultOrder;
}

// virtual
bool CProgressionAction::execute() const
{
  bool success = true;
  static CMetadata Info("StateChange", true);

  try
    {
      CNode * pTarget = const_cast< CNode * >(mpTarget);

      if (CValueInterface(pTarget->healthState) == mStateAtScheduleTime)
        {
          success &= COperation::execute< CNode, const CProgression * >(pTarget, mpProgression, &CNode::set, CNodeProperty:: Collectors[(size_t) CNodeProperty::Property::healthState], Info);
        }
    }
  catch (...)
    {
      CLogger::error("CProgressionAction: Unable to execute action for '{}'.", mpTarget->id);
      success = false;
    }

  return success;
}
