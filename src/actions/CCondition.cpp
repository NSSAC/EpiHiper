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

#include <jansson.h>

#include "actions/CCondition.h"
#include "math/CSizeOf.h"
#include "variables/CVariable.h"


CCondition::CCondition()
{}

CCondition::CCondition(const CCondition & /* src */)
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

CComparison::CComparison(CConditionDefinition::ComparisonType operation, CValueInterface const * pLeft, bool inheritLeft, CValueInterface const * pRight, bool inheritRight)
  : CCondition()
  , mpComparison(NULL)
  , mOwnLeft(inheritLeft)
  , mpLeft(pLeft)
  , mOwnRight(inheritRight)
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
      const_cast< CVariable * >(static_cast< const CVariable * >(mpLeft))->getValue();
    }

  if (dynamic_cast< const CVariable * >(mpRight))
    {
      const_cast< CVariable * >(static_cast< const CVariable * >(mpRight))->getValue();
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

    case CConditionDefinition::ComparisonType::Within:
    case CConditionDefinition::ComparisonType::NotWithin:
      mpComparison = NULL;
      break;
    }
}

CContainedIn::CContainedIn(CConditionDefinition::ComparisonType operation,
                           const CValueInterface * pValue,
                           bool inheritLeft,
                           const CValueList & valueList)
  : CCondition()
  , mpWithin(NULL)
  , mOwnLeft(inheritLeft)
  , mpLeft(pValue)
  , mValueList(valueList)
{
  switch (operation)
    {
    case CConditionDefinition::ComparisonType::Equal:
    case CConditionDefinition::ComparisonType::NotEqual:
    case CConditionDefinition::ComparisonType::Less:
    case CConditionDefinition::ComparisonType::LessOrEqual:
    case CConditionDefinition::ComparisonType::Greater:
    case CConditionDefinition::ComparisonType::GreaterOrEqual:
      mpWithin = NULL;
      break;

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

    case CConditionDefinition::BooleanOperationType::Comparison:
    case CConditionDefinition::BooleanOperationType::Value:
      mpOperation = NULL;
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
