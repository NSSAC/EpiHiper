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
bool CCondition::isTrue(CConditionDefinition::ComparisonType operation, const CValueInterface & Left, const CValueInterface & Right)
{
  bool result = true;

  switch (operation)
    {
    case CConditionDefinition::ComparisonType::Equal:
      result = (Left == Right);
      break;

    case CConditionDefinition::ComparisonType::NotEqual:
      result = (Left != Right);
      break;

    case CConditionDefinition::ComparisonType::Less:
      result = (Left < Right);
      break;

    case CConditionDefinition::ComparisonType::LessOrEqual:
      result = (Left <= Right);
      break;

    case CConditionDefinition::ComparisonType::Greater:
      result = (Left > Right);
      break;

    case CConditionDefinition::ComparisonType::GreaterOrEqual:
      result = (Left >= Right);
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
bool CCondition::isTrue(CConditionDefinition::ComparisonType operation, const CValueInterface & value, const CValueList & valueList)
{
  bool result = true;

  switch (operation)
  {
    case CConditionDefinition::ComparisonType::Within:
      result = valueList.contains(value);
      break;

    case CConditionDefinition::ComparisonType::NotWithin:
      result = !valueList.contains(value);
      break;

    default:
      result = false;
      break;
  }

  return result;
}
