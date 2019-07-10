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

#ifndef SRC_ACTIONS_OPERATION_H_
#define SRC_ACTIONS_OPERATION_H_

struct json_t;

class Operation
{
public:
  virtual ~Operation() {};

  virtual bool execute() const {return false;}
};

template <class Target, class Value> class OperationInstance : public Operation
{
private:
  mutable Target  mTarget;
  Value mValue;
  bool(Target::*mMethod)(Value);

public:
  OperationInstance() = delete;

  OperationInstance(Target target,
                    const Value & value,
                    bool(Target::*method)(Value))
    : mTarget(target)
    , mValue(value)
    , mMethod(method)
  {}

  OperationInstance(const OperationInstance & src)
    : mTarget(src.mTarget)
    , mValue(src.mValue)
    , mMethod(src.mMethod)
  {}

  virtual ~OperationInstance() {};

  virtual bool execute() const
  {
   return (mTarget.*mMethod)(mValue);
  }
};

class OperationDefinition
{
public:
  enum struct TargetType
  {
    node,
    edge,
    variable
  };

  /**
   * Default constructor
   */
  OperationDefinition();

  /**
   * Copy construnctor
   * @param const Feature & src
   */
  OperationDefinition(const OperationDefinition & src);

  /**
   * Destructor
   */
  virtual ~OperationDefinition();

  void fromJSON(const json_t * json);

  template < class Target > Operation createOperation(Target * pTarget) const;
};

template < class Target > Operation OperationDefinition::createOperation(Target * pTarget) const
{
  // TODO CRITICAL Implement me!
  return Operation();
}

#endif /* SRC_ACTIONS_OPERATION_H_ */
