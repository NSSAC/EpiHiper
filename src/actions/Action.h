// BEGIN: Copyright 
// Copyright (C) 2019 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#ifndef SRC_ACTIONS_ACTION_H_
#define SRC_ACTIONS_ACTION_H_

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

#include <actions/Condition.h>
#include <actions/Operation.h>

class Action
{
public:
  Action() = delete;
  Action(const double & priority,
         const Condition & condition,
         const std::vector< Operation > & operations);
  Action(const Action & src);
  virtual ~Action();

  double getPriority() const;
  const Condition & getCondition() const;
  const std::vector< Operation > & getOperations() const;

private:
  double mPriority;
  Condition mCondition;
  std::vector< Operation > mOperations;
};

class ActionDefinition
{
public:
  /**
   * Default constructor
   */
  ActionDefinition();

  /**
   * Copy construnctor
   * @param const Feature & src
   */
  ActionDefinition(const ActionDefinition & src);

  /**
   * Destructor
   */
  virtual ~ActionDefinition();

  void fromJSON(const json_t * json);
};

#endif /* SRC_ACTIONS_ACTION_H_ */
