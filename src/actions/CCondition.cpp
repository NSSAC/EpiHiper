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

#include <jansson.h>

#include "actions/CCondition.h"
#include "math/CSizeOf.h"
#include "variables/CVariable.h"


CCondition::CCondition()
{}

CCondition::CCondition(const CCondition & src)
{}

// virtual
CCondition::~CCondition()
{}

CBooleanValue::CBooleanValue(const bool & value)
  : CCondition()
  , mTrue(value)
{}

// virtual
CBooleanValue::~CBooleanValue()
{}

// virtual
bool CBooleanValue::isTrue() const
{
  return mTrue;
}

CComparison::CComparison(CConditionDefinition::ComparisonType operation, CValueInterface const & left, CValueInterface const & right)
  : CCondition()
  , mpComparison(NULL)
  , mOwnLeft(true)
  , mpLeft(left.copy())
  , mOwnRight(true)
  , mpRight(right.copy())
{
  selectComparison(operation);
}

CComparison::CComparison(CConditionDefinition::ComparisonType operation, CValueInterface const & left, CValueInterface const * pRight)
  : CCondition()
  , mpComparison(NULL)
  , mOwnLeft(true)
  , mpLeft(left.copy())
  , mOwnRight(false)
  , mpRight(pRight)
{
  selectComparison(operation);
}

CComparison::CComparison(CConditionDefinition::ComparisonType operation, CValueInterface const * pLeft, CValueInterface const & right)
  : CCondition()
  , mpComparison(NULL)
  , mOwnLeft(false)
  , mpLeft(pLeft)
  , mOwnRight(true)
  , mpRight(right.copy())
{
  selectComparison(operation);
}

CComparison::CComparison(CConditionDefinition::ComparisonType operation, CValueInterface const * pLeft, CValueInterface const * pRight)
  : CCondition()
  , mpComparison(NULL)
  , mOwnLeft(false)
  , mpLeft(pLeft)
  , mOwnRight(false)
  , mpRight(pRight)
{
  selectComparison(operation);
}

// virtual
CComparison::~CComparison()
{
  if (mOwnLeft) delete mpLeft;
  if (mOwnRight) delete mpRight;
}

// virtual
bool CComparison::isTrue() const
{
  if (dynamic_cast< const CVariable * >(mpLeft))
    {
      const_cast< CVariable * >(static_cast< const CVariable * >(mpLeft))->process();
    }

  if (dynamic_cast< const CVariable * >(mpRight))
    {
      const_cast< CVariable * >(static_cast< const CVariable * >(mpRight))->process();
    }


  return (*mpComparison)(*mpLeft, *mpRight);
}

void CComparison::selectComparison(CConditionDefinition::ComparisonType operation)
{
  switch (operation)
  {
    case CConditionDefinition::ComparisonType::Equal:
      mpComparison = &operator==;
      break;

    case CConditionDefinition::ComparisonType::NotEqual:
      mpComparison = &operator!=;
      break;

    case CConditionDefinition::ComparisonType::Less:
      mpComparison = &operator<;
      break;

    case CConditionDefinition::ComparisonType::LessOrEqual:
      mpComparison = &operator<=;
      break;

    case CConditionDefinition::ComparisonType::Greater:
      mpComparison = &operator>;
      break;

    case CConditionDefinition::ComparisonType::GreaterOrEqual:
      mpComparison = &operator>=;
      break;
  }
}

CContainedIn::CContainedIn(CConditionDefinition::ComparisonType operation,
                                                                         const CValueInterface & value,
                                                                         const CValueList & valueList)
  : CCondition()
  , mpWithin(NULL)
  , mOwnLeft(true)
  , mpLeft(value.copy())
  , mValueList(valueList)
{
  switch (operation)
  {
    case CConditionDefinition::ComparisonType::Within:
      mpWithin = &within;
      break;

    case CConditionDefinition::ComparisonType::NotWithin:
      mpWithin = &notWithin;
      break;
  }
}

CContainedIn::CContainedIn(CConditionDefinition::ComparisonType operation,
                                                                         const CValueInterface * pValue,
                                                                         const CValueList & valueList)
  : CCondition()
  , mpWithin(NULL)
  , mOwnLeft(false)
  , mpLeft(pValue)
  , mValueList(valueList)
{
  switch (operation)
  {
    case CConditionDefinition::ComparisonType::Within:
      mpWithin = &within;
      break;

    case CConditionDefinition::ComparisonType::NotWithin:
      mpWithin = &notWithin;
      break;
   }
}

// virtual
CContainedIn::~CContainedIn()
{
  if (mOwnLeft) delete mpLeft;
}

bool CContainedIn::within(const CValueInterface & value, const CValueList & valueList)
{
  return valueList.contains(value);
}

// static
bool CContainedIn::notWithin(const CValueInterface & value, const CValueList & valueList)
{
  return !valueList.contains(value);
}

// virtual
bool CContainedIn::isTrue() const
{
  return (*mpWithin)(*mpLeft, mValueList);
}

bool CBooleanOperation::_and() const
{
  bool Result = true;

  std::vector< CCondition * >::const_iterator it = mVector.begin();
  std::vector< CCondition * >::const_iterator end = mVector.end();

  for (; it != end && Result; ++it)
    Result &= (*it)->isTrue();

  return Result;
}

bool CBooleanOperation::_or() const
{
  bool Result = false;

  std::vector< CCondition * >::const_iterator it = mVector.begin();
  std::vector< CCondition * >::const_iterator end = mVector.end();

  for (; it != end && !Result; ++it)
    Result |= (*it)->isTrue();

  return Result;

}

// static
bool CBooleanOperation::_not() const
{
  return !(*mVector.begin())->isTrue();
}

CBooleanOperation::CBooleanOperation(CConditionDefinition::BooleanOperationType operation,
                                                 const std::vector< CCondition * > & booleanVector)
  : mpOperation(NULL)
  , mVector(booleanVector)
{
  switch (operation)
  {
    case CConditionDefinition::BooleanOperationType::And:
      mpOperation = &CBooleanOperation::_and;
      break;

    case CConditionDefinition::BooleanOperationType::Or:
      mpOperation = &CBooleanOperation::_or;
      break;

    case CConditionDefinition::BooleanOperationType::Not:
      mpOperation = &CBooleanOperation::_not;
      break;
  }
}

// virtual
CBooleanOperation::~CBooleanOperation()
{
  std::vector< CCondition * >::iterator it = mVector.begin();
  std::vector< CCondition * >::iterator end = mVector.end();

  for (; it != end; ++it)
    delete *it;
}

// virtual
bool CBooleanOperation::isTrue() const
{
  return (this->*mpOperation)();
}
