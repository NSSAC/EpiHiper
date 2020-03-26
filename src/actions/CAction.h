// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#ifndef SRC_ACTIONS_CACTION_H_
#define SRC_ACTIONS_CACTION_H_

/*
class Action {
private:
  bool conditionsTrue;
  std::vector<ActionCondition> conditions;
public:
  Action();
  ~Action();
  checkConditions();
  conditionsTrue();
};
 */

#include <vector>

#include "actions/CCondition.h"
#include "actions/COperation.h"

class CAction
{
public:
  CAction() = delete;
  CAction(const double & priority,
         CCondition * pCondition);
  CAction(const CAction & src) = delete;
  virtual ~CAction();

  double getPriority() const;
  void addOperation(COperation * pOperation);

  bool execute() const;

  void toBinary(std::ostream & os) const;
  bool fromBinary(std::istream & is);

private:
  double mPriority;
  CCondition * mpCondition;
  std::vector< COperation * > mOperations;
};

#endif /* SRC_ACTIONS_CACTION_H_ */
