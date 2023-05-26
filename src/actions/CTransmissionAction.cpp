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

#include "actions/CTransmissionAction.h"
#include "actions/COperation.h"
#include "diseaseModel/CTransmission.h"
#include "diseaseModel/CHealthState.h"
#include "network/CNode.h"
#include "network/CEdge.h"
#include "utilities/CMetadata.h"
#include "utilities/CLogger.h"

CTransmissionAction::CTransmissionAction(const CTransmission * pTransmission, const CNode * pTarget, const CEdge * pEdge)
  : CAction()
  , mpTransmission(pTransmission)
  , mpTarget(pTarget)
  , mStateAtScheduleTime(pTarget->healthState)
  , mpEdge(pEdge)
{
  CLogger::trace Trace;
  Trace << "CTransmissionAction: Add node "
          << mpTarget->id
          << " healthState = "
          << mpTransmission->getExitState()->getId()
          << ", contact: "
          << mpEdge->pSource->id;

#ifdef USE_LOATION_ID
  if (CEdge::HasLocationId)
    {
      Trace << ", location: "
            << mpEdge->locationId;
    }
#endif
}

// virtual
CTransmissionAction::~CTransmissionAction()
{}

// virtual
size_t CTransmissionAction::getOrder() const
{
  return 1;
}

// virtual
bool CTransmissionAction::execute() const
{
  bool success = true;

  try
    {
      CNode * pTarget = const_cast< CNode * >(mpTarget);

      if (CValueInterface(pTarget->healthState) == mStateAtScheduleTime)
        {
          CMetadata Info("StateChange", true);
          Info.set("ContactNode", (int) mpEdge->pSource->id);

#ifdef USE_LOCATION_ID
          if (CEdge::HasLocationId)
            {
              Info.set("LocationId", (int) mpEdge->locationId);
            }
#endif

          success &= COperation::execute< CNode, const CTransmission * >(pTarget, mpTransmission, &CNode::set, CNodeProperty::Collectors[(size_t) CNodeProperty::Property::healthState], Info);
        }
    }
  catch (...)
    {
      CLogger::error() << "CTransmissionAction: Unable to execute action for '" << mpTarget->id << "'.";
      success = false;
    }

  return success;
}
