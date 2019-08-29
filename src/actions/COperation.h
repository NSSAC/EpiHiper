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

class CMetadata;

struct json_t;

class COperation
{
public:
  virtual ~COperation() {};

  virtual bool execute(const CMetadata & metadata) const = 0;
  virtual COperation * copy() const = 0;

  void toBinary(std::ostream & os) const;
  void fromBinary(std::istream & is);
};

template <class Target, class Value> class COperationInstance : public COperation
{
private:
  mutable Target  * mpTarget;
  Value mValue;
  bool(Target::*mMethod)(Value, const CMetadata & metadata);

public:
  COperationInstance() = delete;

  COperationInstance(Target *pTarget,
                    Value value,
                    bool(Target::*method)(Value, const CMetadata &))
    : mpTarget(pTarget)
    , mValue(value)
    , mMethod(method)
  {}

  COperationInstance(const COperationInstance & src)
    : mpTarget(src.mpTarget)
    , mValue(src.mValue)
    , mMethod(src.mMethod)
  {}

  virtual ~COperationInstance() {};

  virtual bool execute(const CMetadata & metadata) const
  {
    bool changed = (mpTarget->*mMethod)(mValue, metadata);

    if (changed)
      {
        Changes::record(*mpTarget, metadata);
      }

    return changed;
  }

  virtual COperation * copy() const
  {
    return new COperationInstance<Target, Value>(*this);
  }
};

#endif /* SRC_ACTIONS_COPERATION_H_ */
