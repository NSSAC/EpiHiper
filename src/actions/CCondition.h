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

/*
 * Condition.h
 *
 *  Created on: Jul 1, 2019
 *      Author: shoops
 */

#ifndef SRC_ACTIONS_CCONDITION_H_
#define SRC_ACTIONS_CCONDITION_H_

#include <vector>

#include "actions/CConditionDefinition.h"
#include "math/CComputable.h"

struct json_t;
class CHealthState;

class CCondition
{
protected:
  CCondition();

public:
  CCondition(const CCondition & src);
  virtual ~CCondition();

  virtual bool isTrue() const = 0;
};

class CComparison : public CCondition
{
public:
  typedef bool (*pComparison)(const CValueInterface &, const CValueInterface &);

  CComparison() = delete;

  CComparison(CConditionDefinition::ComparisonType operation, CValueInterface const * pLeft, bool inheritLeft, CValueInterface const * pRight, bool inheritRight);

  CComparison(const CComparison & src) = delete;

  virtual ~CComparison();

  virtual bool isTrue() const override;

private:
  void selectComparison(CConditionDefinition::ComparisonType operation);

  pComparison mpComparison;
  bool mOwnLeft;
  CValueInterface const * mpLeft;
  bool mOwnRight;
  CValueInterface const * mpRight;
};

class CBooleanValue : public CCondition
{
public:
  CBooleanValue() = delete;
  CBooleanValue(const bool & value);
  CBooleanValue(const CBooleanValue & src) = delete;
  virtual ~CBooleanValue();

  virtual bool isTrue() const override;

private:
  bool mTrue;
};

class CBooleanOperation : public CCondition
{
private:
  bool _and() const;
  bool _or() const;
  bool _not() const;

public:
  typedef bool (CBooleanOperation::*pOperation)() const;

  CBooleanOperation() = delete;
  CBooleanOperation(CConditionDefinition::BooleanOperationType operation, const std::vector< CCondition * > & booleanVector);
  CBooleanOperation(const CBooleanOperation & src) = delete;
  virtual ~CBooleanOperation();

  virtual bool isTrue() const override;
private:
  pOperation mpOperation;
  std::vector< CCondition * > mVector;
};

class CContainedIn : public CCondition
{
private:
  static bool within(const CValueInterface &, const CValueList &);
  static bool notWithin(const CValueInterface &, const CValueList &);

public:
  typedef bool (*pWithin)(const CValueInterface &, const CValueList &);

  CContainedIn() = delete;
  CContainedIn(CConditionDefinition::ComparisonType operation, const CValueInterface * pValue, bool inheritLeft, const CValueList & set);
  CContainedIn(const CContainedIn & src) = delete;
  virtual ~CContainedIn();

  virtual bool isTrue() const override;

private:
  pWithin mpWithin;
  bool mOwnLeft;
  CValueInterface const * mpLeft;
  const CValueList & mValueList;
};

#endif /* SRC_ACTIONS_CCONDITION_H_ */
