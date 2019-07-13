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

#include "actions/Changes.h"

class Metadata;

struct json_t;

class Operation
{
public:
  virtual ~Operation() {};

  virtual bool execute(const Metadata & metadata) const = 0;
  virtual Operation * copy() const = 0;
};

template <class Target, class Value> class OperationInstance : public Operation
{
private:
  mutable Target  mTarget;
  Value mValue;
  bool(Target::*mMethod)(Value, const Metadata & metadata);

public:
  OperationInstance() = delete;

  OperationInstance(Target target,
                    const Value & value,
                    bool(Target::*method)(Value, const Metadata & metadata))
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

  virtual bool execute(const Metadata & metadata) const
  {
    bool changed = (mTarget.*mMethod)(mValue, metadata);

    if (changed)
      {
        Changes::record(mTarget, metadata);
      }

    return changed;
  }

  virtual Operation * copy() const
  {
    return new OperationInstance<Target, Value>(*this);
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

  template < class Target > Operation * createOperation(Target * pTarget) const;
};

template < class Target > Operation * OperationDefinition::createOperation(Target * pTarget) const
{
  // TODO CRITICAL Implement me!
  return NULL;
}

#endif /* SRC_ACTIONS_OPERATION_H_ */
