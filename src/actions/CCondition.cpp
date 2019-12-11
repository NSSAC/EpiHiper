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


CBoolean::CBoolean()
{}

CBoolean::CBoolean(const CBoolean & src)
{}

// virtual
CBoolean::~CBoolean()
{}

CCondition::CCondition(const CBoolean & boolean)
  : CBoolean()
  , mpBoolean(boolean.copy())
{}

CCondition::CCondition(const CCondition & src)
  : CBoolean(src)
  , mpBoolean(src.mpBoolean->copy())
{}

// virtual
CCondition::~CCondition()
{
  if (mpBoolean != NULL) delete mpBoolean;
}

// virtual
CBoolean * CCondition::copy() const
{
  return new CCondition(*this);
}

bool CCondition::isTrue() const
{
  return mpBoolean->isTrue();
}

CCondition::CBooleanValue::CBooleanValue(const bool & value)
  : CBoolean()
  , mTrue(value)
{}

CCondition::CBooleanValue::CBooleanValue(const CBooleanValue & src)
  : CBoolean(src)
  , mTrue(src.mTrue)
{}

// virtual
CCondition::CBooleanValue::~CBooleanValue()
{}

// virtual
CBoolean * CCondition::CBooleanValue::copy() const
{
  return new CBooleanValue(*this);
}

// virtual
bool CCondition::CBooleanValue::isTrue() const
{
  return mTrue;
}

CCondition::CComparison::CComparison(CConditionDefinition::ComparisonType operation, CValueInterface const & left, CValueInterface const & right)
  : CBoolean()
  , mpComparison(NULL)
  , mOwnLeft(true)
  , mpLeft(left.copy())
  , mOwnRight(true)
  , mpRight(right.copy())
{
  selectComparison(operation);
}

CCondition::CComparison::CComparison(CConditionDefinition::ComparisonType operation, CValueInterface const & left, CValueInterface const * pRight)
  : CBoolean()
  , mpComparison(NULL)
  , mOwnLeft(true)
  , mpLeft(left.copy())
  , mOwnRight(false)
  , mpRight(pRight)
{
  selectComparison(operation);
}

CCondition::CComparison::CComparison(CConditionDefinition::ComparisonType operation, CValueInterface const * pLeft, CValueInterface const & right)
  : CBoolean()
  , mpComparison(NULL)
  , mOwnLeft(false)
  , mpLeft(pLeft)
  , mOwnRight(true)
  , mpRight(right.copy())
{
  selectComparison(operation);
}

CCondition::CComparison::CComparison(CConditionDefinition::ComparisonType operation, CValueInterface const * pLeft, CValueInterface const * pRight)
  : CBoolean()
  , mpComparison(NULL)
  , mOwnLeft(false)
  , mpLeft(pLeft)
  , mOwnRight(false)
  , mpRight(pRight)
{
  selectComparison(operation);
}

CCondition::CComparison::CComparison(const CComparison & src)
  : CBoolean(src)
  , mpComparison(src.mpComparison)
  , mOwnLeft(src.mOwnLeft)
  , mpLeft(src.mOwnLeft ? src.mpLeft->copy(): src.mpLeft)
  , mOwnRight(src.mOwnRight)
  , mpRight(src.mOwnRight ? src.mpRight->copy(): src.mpRight)
{}

// virtual
CCondition::CComparison::~CComparison()
{
  if (mOwnLeft) delete mpLeft;
  if (mOwnRight) delete mpRight;
}

// virtual
CBoolean * CCondition::CComparison::copy() const
{
  return new CComparison(*this);
}

// virtual
bool CCondition::CComparison::isTrue() const
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

void CCondition::CComparison::selectComparison(CConditionDefinition::ComparisonType operation)
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

CCondition::CContainedIn::CContainedIn(CConditionDefinition::ComparisonType operation,
                                                                         const CValueInterface & value,
                                                                         const CValueList & valueList)
  : CBoolean()
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

CCondition::CContainedIn::CContainedIn(CConditionDefinition::ComparisonType operation,
                                                                         const CValueInterface * pValue,
                                                                         const CValueList & valueList)
  : CBoolean()
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
CCondition::CContainedIn::CContainedIn(const CContainedIn & src)
  : CBoolean(src)
  , mpWithin(src.mpWithin)
  , mOwnLeft(src.mOwnLeft)
  , mpLeft(src.mOwnLeft ? new CValueInterface(*src.mpLeft): src.mpLeft)
  , mValueList(src.mValueList)
{}

// virtual
CCondition::CContainedIn::~CContainedIn()
{
  if (mOwnLeft) delete mpLeft;
}

// virtual
CBoolean * CCondition::CContainedIn::copy() const
{
  return new CContainedIn(*this);
}

bool CCondition::CContainedIn::within(const CValueInterface & value, const CValueList & valueList)
{
  return valueList.contains(value);
}

// static
bool CCondition::CContainedIn::notWithin(const CValueInterface & value, const CValueList & valueList)
{
  return !valueList.contains(value);
}

// virtual
bool CCondition::CContainedIn::isTrue() const
{
  return (*mpWithin)(*mpLeft, mValueList);
}

bool CCondition::CBooleanOperation::_and() const
{
  bool Result = true;

  std::vector< CBoolean * >::const_iterator it = mVector.begin();
  std::vector< CBoolean * >::const_iterator end = mVector.end();

  for (; it != end && Result; ++it)
    Result &= (*it)->isTrue();

  return Result;
}

bool CCondition::CBooleanOperation::_or() const
{
  bool Result = false;

  std::vector< CBoolean * >::const_iterator it = mVector.begin();
  std::vector< CBoolean * >::const_iterator end = mVector.end();

  for (; it != end && !Result; ++it)
    Result |= (*it)->isTrue();

  return Result;

}

// static
bool CCondition::CBooleanOperation::_not() const
{
  return !(*mVector.begin())->isTrue();
}

CCondition::CBooleanOperation::CBooleanOperation(CConditionDefinition::BooleanOperationType operation,
                                                 const std::vector< CBoolean * > & booleanVector)
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

CCondition::CBooleanOperation::CBooleanOperation(const CBooleanOperation & src)
  : mpOperation(src.mpOperation)
  , mVector()
{
  std::vector< CBoolean * >::const_iterator it = src.mVector.begin();
  std::vector< CBoolean * >::const_iterator end = src.mVector.end();

  for (; it != end; ++it)
    mVector.push_back((*it)->copy());
}

// virtual
CCondition::CBooleanOperation::~CBooleanOperation()
{
  std::vector< CBoolean * >::iterator it = mVector.begin();
  std::vector< CBoolean * >::iterator end = mVector.end();

  for (; it != end; ++it)
    delete *it;
}

// virtual
CBoolean * CCondition::CBooleanOperation::copy() const
{
  return new CBooleanOperation(*this);
}

// virtual
bool CCondition::CBooleanOperation::isTrue() const
{
  return (this->*mpOperation)();
}
