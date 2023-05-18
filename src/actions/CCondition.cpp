// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2023 Rector and Visitors of the University of Virginia 
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

#include <memory>

#include <jansson.h>

#include "actions/CCondition.h"
#include "variables/CVariable.h"

// static 
bool CCondition::isTrue(CConditionDefinition::ComparisonType operation, CValueInterface const * pLeft, CValueInterface const * pRight)
{
  bool result = true;

  std::shared_ptr< CValue > VariableLeft;
  std::shared_ptr< CValue > VariableRight;

  if (dynamic_cast< const CVariable * >(pLeft))
    VariableLeft = std::make_shared< CValue >(const_cast< CVariable * >(static_cast< const CVariable * >(pLeft))->toValue());

  if (dynamic_cast< const CVariable * >(pRight))
    VariableRight = std::make_shared< CValue >(const_cast< CVariable * >(static_cast< const CVariable * >(pRight))->toValue());

  const CValueInterface & ValueLeft = VariableLeft ? *VariableLeft : *pLeft;
  const CValueInterface & ValueRight = VariableLeft ? *VariableRight : *pRight;

  switch (operation)
    {
    case CConditionDefinition::ComparisonType::Equal:
      result = (ValueLeft == ValueRight);
      break;

    case CConditionDefinition::ComparisonType::NotEqual:
      result = (ValueLeft != ValueRight);
      break;

    case CConditionDefinition::ComparisonType::Less:
      result = (ValueLeft < ValueRight);
      break;

    case CConditionDefinition::ComparisonType::LessOrEqual:
      result = (ValueLeft <= ValueRight);
      break;

    case CConditionDefinition::ComparisonType::Greater:
      result = (ValueLeft > ValueRight);
      break;

    case CConditionDefinition::ComparisonType::GreaterOrEqual:
      result = (ValueLeft >= ValueRight);
      break;

    default:
      result = false;
      break;
    }

  return result;
}

// static 
bool CCondition::isTrue(const bool & value)
{
  return value;
}

// static 
bool CCondition::isTrue(CConditionDefinition::BooleanOperationType operation, const std::vector< bool > & booleanVector)
{
  bool result = true;

    switch (operation)
    {
      case CConditionDefinition::BooleanOperationType::And:
        for (const bool & value : booleanVector)
          result &= value;
        break;

      case CConditionDefinition::BooleanOperationType::Or:
        for (const bool & value : booleanVector)
          result |= value;
        break;

      case CConditionDefinition::BooleanOperationType::Not:
        result = !booleanVector[0];
        break;

      default:
        result = false;
        break;
  }

  return result;
}

// static 
bool CCondition::isTrue(CConditionDefinition::ComparisonType operation, const CValueInterface * pValue, const CValueList & valueList)
{
  bool result = true;
  std::shared_ptr< CValue > VariableLeft;

  if (dynamic_cast< const CVariable * >(pValue))
    VariableLeft = std::make_shared< CValue >(const_cast< CVariable * >(static_cast< const CVariable * >(pValue))->toValue());

  const CValueInterface & Value = VariableLeft ? *VariableLeft : *pValue;

  switch (operation)
  {
    case CConditionDefinition::ComparisonType::Within:
      result = valueList.contains(Value);
      break;

    case CConditionDefinition::ComparisonType::NotWithin:
      result = !valueList.contains(Value);
      break;

    default:
      result = false;
      break;
  }

  return result;
}
