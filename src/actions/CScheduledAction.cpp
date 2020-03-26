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

#include "actions/CScheduledAction.h"
#include "actions/CActionDefinition.h"
#include "actions/CAction.h"

CScheduledAction::CScheduledAction(CAction * pAction)
  : mKind(Kind::Action)
  , mpDefinition(NULL)
  , mpTarget(pAction)
{}

CScheduledAction::CScheduledAction(const CActionDefinition * pActionDefinition, const CNode * pNode)
  : mKind(Kind::Node)
  , mpDefinition(pActionDefinition)
  , mpTarget(const_cast< CNode * >(pNode))
{}

CScheduledAction::CScheduledAction(const CActionDefinition * pActionDefinition, const CEdge * pEdge)
  : mKind(Kind::Edge)
  , mpDefinition(pActionDefinition)
  , mpTarget(const_cast< CEdge * >(pEdge))
{}

CScheduledAction::CScheduledAction(const CActionDefinition * pActionDefinition)
  : mKind(Kind::Other)
  , mpDefinition(pActionDefinition)
  , mpTarget(NULL)
{}

CScheduledAction::~CScheduledAction()
{
  switch (mKind)
  {
    case Kind::Action:
      delete static_cast< CAction * >(mpTarget);
      break;

    case Kind::Node:
    case Kind::Edge:
    case Kind::Other:
      break;
  }
}

double CScheduledAction::getPriority() const
{
  switch (mKind)
  {
    case Kind::Action:
      return static_cast< CAction * >(mpTarget)->getPriority();
      break;

    case Kind::Node:
    case Kind::Edge:
    case Kind::Other:
      return mpDefinition->getPriority();
      break;
  }

  return 0.0;
}

bool CScheduledAction::execute() const
{
  switch (mKind)
  {
    case Kind::Action:
      return static_cast< CAction * >(mpTarget)->execute();
      break;

    case Kind::Node:
      return mpDefinition->execute(static_cast< CNode * >(mpTarget));
      break;

    case Kind::Edge:
      return mpDefinition->execute(static_cast< CEdge * >(mpTarget));
      break;
      
    case Kind::Other:
      return mpDefinition->execute();
      break;
  }

  return false;
}
