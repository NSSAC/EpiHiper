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
double CTransmissionAction::getPriority() const
{
  return 1.0;
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

          success &= COperationInstance< CNode, const CTransmission * >(pTarget, mpTransmission, NULL, &CNode::set, Info).execute();
        }
    }
  catch (...)
    {
      CLogger::error() << "CTransmissionAction: Unable to execute action for '" << mpTarget->id << "'.";
      success = false;
    }

  return success;
}
