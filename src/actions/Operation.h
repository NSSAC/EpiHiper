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
  Target * mpTarget;
  Value mValue;
  bool(Target::*mMethod)(const Value &) const;

public:
  OperationInstance() = delete;

  OperationInstance(Target * pTarget,
                    const Value & value,
                    bool(Target::*method)(const Value &) const)
    : mpTarget(pTarget)
    , mValue(value)
    , mMethod(method)
  {}

  OperationInstance(const OperationInstance & src)
    : mpTarget(src.mpTarget)
    , mValue(src.mValue)
    , mMethod(src.mMethod)
  {}

  virtual ~OperationInstance() {};

  virtual bool execute() const
  {
   return (*mpTarget.*mMethod)(mValue);
  }
};

class OperationDefinition
{
public:
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
