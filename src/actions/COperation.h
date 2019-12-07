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

#ifndef SRC_ACTIONS_COPERATION_H_
#define SRC_ACTIONS_COPERATION_H_

#include "actions/Changes.h"
#include "utilities/CMetadata.h"
#include "math/CValueInterface.h"

struct json_t;

class COperation
{
public:
  virtual ~COperation() {};

  virtual bool execute() const = 0;
  virtual COperation * copy() const = 0;

  void toBinary(std::ostream & os) const;
  void fromBinary(std::istream & is);
};

template <class Target, class Value> class COperationInstance : public COperation
{
private:
  mutable Target  * mpTarget;
  Value mValue;
  CValueInterface::pOperator mpOperator;
  bool(Target::*mMethod)(Value, CValueInterface::pOperator pOperator, const CMetadata & metadata);
  CMetadata mMetadata;

public:
  COperationInstance() = delete;

  COperationInstance(Target *pTarget,
                    Value value,
                    CValueInterface::pOperator pOperator,
                    bool(Target::*method)(Value, CValueInterface::pOperator, const CMetadata &),
                    const CMetadata & metadata = CMetadata())
    : mpTarget(pTarget)
    , mValue(value)
    , mpOperator(pOperator)
    , mMethod(method)
    , mMetadata()
  {}

  COperationInstance(const COperationInstance & src)
    : mpTarget(src.mpTarget)
    , mValue(src.mValue)
    , mpOperator(src.mpOperator)
    , mMethod(src.mMethod)
    , mMetadata(src.mMetadata)
  {}

  virtual ~COperationInstance() {};

  virtual bool execute() const
  {
    bool changed = (mpTarget->*mMethod)(mValue, mpOperator, mMetadata);

    if (changed)
      {
        Changes::record(mpTarget, mMetadata);
      }

    return changed;
  }

  virtual COperation * copy() const
  {
    return new COperationInstance<Target, Value>(*this);
  }
};

#endif /* SRC_ACTIONS_COPERATION_H_ */
