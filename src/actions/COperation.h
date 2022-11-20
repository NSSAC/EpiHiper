// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
//  
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal 
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions: 
//  
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software. 
//  
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE 
// END: Copyright 

#ifndef SRC_ACTIONS_COPERATION_H_
#define SRC_ACTIONS_COPERATION_H_

#include "actions/CChanges.h"
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

  COperationInstance(Target * pTarget,
                     Value value,
                     CValueInterface::pOperator pOperator,
                     bool (Target::*method)(Value, CValueInterface::pOperator, const CMetadata &),
                     const CMetadata & metadata = CMetadata())
    : mpTarget(pTarget)
    , mValue(value)
    , mpOperator(pOperator)
    , mMethod(method)
    , mMetadata(metadata)
  {}

  COperationInstance(const COperationInstance & src)
    : mpTarget(src.mpTarget)
    , mValue(src.mValue)
    , mpOperator(src.mpOperator)
    , mMethod(src.mMethod)
    , mMetadata(src.mMetadata)
  {}

  virtual ~COperationInstance() {};

  virtual bool execute() const override
  {
    bool changed = (mpTarget->*mMethod)(mValue, mpOperator, mMetadata);

    if (changed)
      {
        CChanges::record(mpTarget, mMetadata);
      }

    return changed;
  }

  virtual COperation * copy() const override
  {
    return new COperationInstance<Target, Value>(*this);
  }
};

#endif /* SRC_ACTIONS_COPERATION_H_ */
