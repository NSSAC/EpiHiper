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

#ifndef SRC_ACTIONS_CSCHEDULEDACTION_H_
#define SRC_ACTIONS_CSCHEDULEDACTION_H_

class CActionDefinition;
class CAction;
class CNode;
class CEdge;

class CScheduledAction
{
public:
  enum struct Kind
  {
    Action,
    Node,
    Edge,
    Other
  };

  CScheduledAction() = delete;
  CScheduledAction(const CScheduledAction & src) = delete;
  CScheduledAction(CAction * pAction);
  CScheduledAction(const CActionDefinition * pActionDefinition, const CNode * pNode);
  CScheduledAction(const CActionDefinition * pActionDefinition, const CEdge * pEdge);
  CScheduledAction(const CActionDefinition * pActionDefinition);

  ~CScheduledAction();

  double getPriority() const;
  bool execute() const;

private:
  Kind mKind;
  const CActionDefinition * mpDefinition;
  void * mpTarget;
};

#endif // SRC_ACTIONS_CSCHEDULEDACTION_H_