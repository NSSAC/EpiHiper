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

#include <cstring>
#include <jansson.h>

#include "actions/CConditionDefinition.h"
#include "actions/CCondition.h"
#include "math/CObservable.h"
#include "math/CNodeProperty.h"
#include "math/CEdgeProperty.h"
#include "math/CSizeOf.h"
#include "variables/CVariableList.h"
#include "utilities/CLogger.h"
#include "utilities/CSimConfig.h"

CConditionDefinition::CConditionDefinition()
  : mType(BooleanOperationType::Value)
  , mComparison()
  , mLeft()
  , mRight()
  , mValue(true)
  , mBooleanValues()
  , mValid(true)
{}

CConditionDefinition::CConditionDefinition(const CConditionDefinition & src)
  : mType(src.mType)
  , mComparison(src.mComparison)
  , mLeft(src.mLeft)
  , mRight(src.mRight)
  , mValue(src.mValue)
  , mBooleanValues(src.mBooleanValues)
  , mValid(src.mValid)
{}

CConditionDefinition::CConditionDefinition(const json_t * json)
  : mType(BooleanOperationType::__SIZE)
  , mComparison()
  , mLeft()
  , mRight()
  , mValue(false)
  , mBooleanValues()
  , mValid(false)
{
  fromJSON(json);
}

// virtual
CConditionDefinition::~CConditionDefinition()
{}

void CConditionDefinition::fromJSON(const json_t * json)
{
  // This results in recursive parsing the only leaves are
  // {"$ref": "#/definitions/booleanValue"},
  // {"$ref": "#/definitions/comparison"}

  /*
  "boolean": {
    "$id": "#boolean",
    "description": "A boolean value.",
    "type": "object",
    "oneOf": [
      {"$ref": "#/definitions/booleanValue"},
      {"$ref": "#/definitions/booleanNot"},
      {"$ref": "#/definitions/booleanOperation"},
      {"$ref": "#/definitions/comparison"}
    ]
  },
  */

  if (json == NULL)
    {
      mType = BooleanOperationType::Value;
      mValue = true;
      mValid = true;

      return;
    }

  if (valueFromJSON(json))
    return;

  if (comparisonFromJSON(json))
    return;

  if (operationFromJSON(json)) // also handles not
    return;

  CLogger::error("Condition: Invalid. {}", CSimConfig::jsonToString(json));
}

bool CConditionDefinition::valueFromJSON(const json_t * json)
{
  /*
    "booleanValue": {
      "$id": "#booleanValue",
      "description": "A fixed boolean value",
      "type": "object",
      "required": ["value"],
      "properties": {
        "value": {"type": "boolean"}
      }
    },
   */

  mValid = false;
  json_t * pValue = json_object_get(json, "value");

  if (pValue != NULL)
    {
      if (json_is_boolean(pValue))
        {
          mType = BooleanOperationType::Value;
          mValue = json_is_true(pValue);
          mValid = true;
        }
      else
        {
          CLogger::error("Condition: Invalid Boolean value.");
        }

      return true;
    }

  return false;
}

bool CConditionDefinition::operationFromJSON(const json_t * json)
{
  /*
    "booleanNot": {
      "$id": "#booleanNot",
      "description": "Negates the boolean value",
      "type": "object",
      "required": ["not"],
      "properties": {
        "not": {"$ref": "#/definitions/boolean"}
      }
    },
    */

  mValid = false;
  json_t * pOperation = json_object_get(json, "not");

  if (pOperation != NULL)
    {
      if (json_is_object(pOperation))
        {
          mType = BooleanOperationType::Not;
          CConditionDefinition Child(pOperation);

          if (Child.isValid())
            {
              mValid = true;
              mBooleanValues.push_back(Child);
            }
          else
            {
              CLogger::error("Condition: Invalid operant for Boolean operator 'not'.");
            }
        }
      else
        {
          CLogger::error("Condition: Invalid Boolean operator 'not'.");
        }

      return true;
    }

  /*
    "booleanOperation": {
      "$id": "#booleanOperation",
      "description": "A Boolean operation applied to multiple Boolean values which can be 'and' or 'or'.",
      "type": "object",
      "oneOf": [
        {
          "required": ["and"],
          "properties": {
            "and": {
              "type": "array",
              "items": {"$ref": "#/definitions/boolean"}
            }
          },
          "additionalProperties": false
        },
        {
          "required": ["or"],
          "properties": {
            "or": {
              "type": "array",
              "items": {"$ref": "#/definitions/boolean"}
            }
          },
          "additionalProperties": false
        }
      ]
    }
   */
  pOperation = json_object_get(json, "and");

  if (pOperation != NULL)
    {
      if (!json_is_array(pOperation))
        {
          CLogger::error("Condition: Invalid Boolean operator 'and'.");
          return true;
        }

      mType = BooleanOperationType::And;
    }
  else
    {
      pOperation = json_object_get(json, "or");

      if (pOperation != NULL)
        {
          if (!json_is_array(pOperation))
            {
              CLogger::error("Condition: Invalid Boolean operator 'or'.");
            }

          mType = BooleanOperationType::Or;
        }
      else
        {
          return false;
        }
    }

  mValid = true;
  size_t i = 0, imax = json_array_size(pOperation);

  for (; i < imax && mValid; ++i)
    {
      CConditionDefinition Child(json_array_get(pOperation, i));

      if (Child.isValid())
        {
          mBooleanValues.push_back(Child);
        }
      else
        {
          CLogger::error("Condition: Invalid operant for Boolean operator '{}'.", mType == BooleanOperationType::And ? "and" : "or");
          mValid = false; 
        }
    }

  return true;
}

bool CConditionDefinition::comparisonFromJSON(const json_t * json)
{
  /*
    "comparisonOperator": {
      "$id": "#comparisonOperator",
      "description": "Comparison operators",
      "type": "string",
      "enum": [
        "==",
        "!=",
        "<=",
        "<",
        ">=",
        ">"
      ]
    },
    "comparison": {
      "$id": "#comparison",
      "description": "A comparison used in evaluating triggers and action conditions.",
      "oneOf": [
        {
          "description": "Comparing values of nodes, edges, variables, set sizes, and fixed values",
          "type": "object",
          "required": [
            "operator",
            "left",
            "right"
          ],
          "properties": {
            "operator": {"$ref": "#/definitions/comparisonOperator"},
            "left": {
              "type": "object",
              "description": "",
              "oneOf": [
                {"$ref": "#/definitions/value"},
                {"$ref": "#/definitions/observable"},
                {
                  "type": "object",
                  "required": ["node"],
                  "properties": {
                    "node": {"$ref": "#/definitions/nodeProperty"}
                  }
                },
                {
                  "type": "object",
                  "required": ["edge"],
                  "properties": {
                    "edge": {"$ref": "#/definitions/edgeProperty"}
                  }
                },
                {"$ref": "#/definitions/variableReference"},
                {"$ref": "#/definitions/sizeof"}
              ]
            },
            "right": {
              "type": "object",
              "description": "",
              "oneOf": [
                {"$ref": "#/definitions/value"},
                {"$ref": "#/definitions/observable"},
                {
                  "type": "object",
                  "required": ["node"],
                  "properties": {
                    "node": {"$ref": "#/definitions/nodeProperty"}
                  }
                },
                {
                  "type": "object",
                  "required": ["edge"],
                  "properties": {
                    "edge": {"$ref": "#/definitions/edgeProperty"}
                  }
                },
                {"$ref": "#/definitions/variableReference"},
                {"$ref": "#/definitions/sizeof"}
              ]
            }
          }
        },
        {
          "description": "Checking whether a node or edge property is in or not in a given value list.",
          "type": "object",
          "required": [
            "operator",
            "left",
            "right"
          ],
          "properties": {
            "operator": {
              "description": "",
              "type": "string",
              "enum": [
                "in",
                "not in"
              ]
            },
            "left": {
              "type": "object",
              "description": "",
              "oneOf": [
                {
                  "type": "object",
                  "required": ["node"],
                  "properties": {
                    "node": {"$ref": "#/definitions/nodeProperty"}
                  }
                },
                {
                  "type": "object",
                  "required": ["edge"],
                  "properties": {
                    "edge": {"$ref": "#/definitions/edgeProperty"}
                  }
                }
              ]
            },
            "right": {"$ref": "#/definitions/valueList"}
          }
        }
      ]
    },
  */

  mType = BooleanOperationType::Comparison;
  mValid = false;

  json_t * pValue = json_object_get(json, "operator");

  if (pValue == NULL)
    {
      return false;
    }

  if (!json_is_string(pValue))
    {
      CLogger::error("Condition: Invalid comparison operator.");
      return true;
    }

  const char * Comparison = json_string_value(pValue);

  if (strcmp(Comparison, "<") == 0)
    {
      mComparison = ComparisonType::Less;
    }
  else if (strcmp(Comparison, "<=") == 0)
    {
      mComparison = ComparisonType::LessOrEqual;
    }
  else if (strcmp(Comparison, ">") == 0)
    {
      mComparison = ComparisonType::Greater;
    }
  else if (strcmp(Comparison, ">=") == 0)
    {
      mComparison = ComparisonType::GreaterOrEqual;
    }
  else if (strcmp(Comparison, "==") == 0)
    {
      mComparison = ComparisonType::Equal;
    }
  else if (strcmp(Comparison, "!=") == 0)
    {
      mComparison = ComparisonType::NotEqual;
    }
  else if (strcmp(Comparison, "in") == 0)
    {
      mComparison = ComparisonType::Within;
    }
  else if (strcmp(Comparison, "not in") == 0)
    {
      mComparison = ComparisonType::NotWithin;
    }
  else
    {
      CLogger::error("Condition: Invalid comparison operator '{}'.", CSimConfig::jsonToString(json));
      return true;
    }

  mValid = true;

  mLeft.fromJSON(json_object_get(json, "left"), false);

  if (!mLeft.isValid())
    {
      CLogger::error("Condition: Invalid left operant for comparison operator '{}'.", CSimConfig::jsonToString(json));
      mValid = false;
    }

  mRight.fromJSON(json_object_get(json, "right"), false);

  if (!mRight.isValid())
    {
      CLogger::error("Condition: Invalid right operant for comparison operator '{}'.", CSimConfig::jsonToString(json));
      mValid = false;
    }

  if (mComparison != ComparisonType::Within
      && mComparison != ComparisonType::NotWithin)
    {
      if (mLeft.getType() == CValueInstance::ValueType::ValueList)
        {
          CLogger::error("Condition: Invalid value type for left operant for comparison operator '{}'.", CSimConfig::jsonToString(json));
          mValid = false;
        }

      if (mRight.getType() == CValueInstance::ValueType::ValueList)
        {
          CLogger::error("Condition: Invalid value type for right operant for comparison operator '{}'.", CSimConfig::jsonToString(json));
          mValid = false;
        }

      if (!CValueInstance::compatible(mLeft, mRight))
        {
          CLogger::error("Condition: Incompatible values for comparison: '{}'.", CSimConfig::jsonToString(json));
          mValid = false;
        }
    }
  else
    {
      if (mLeft.getType() == CValueInstance::ValueType::ValueList)
        {
          CLogger::error("Condition: Invalid value type for left operant for comparison operator '{}'.", CSimConfig::jsonToString(json));
          mValid = false;
        }

      if (mRight.getType() != CValueInstance::ValueType::ValueList)
        {
          CLogger::error("Condition: Invalid value type for right operant for comparison operator '{}'.", CSimConfig::jsonToString(json));
          mValid = false;
        }

      if (!CValueInstance::compatible(mLeft, mRight))
        {
          CLogger::error("Condition: Incompatible values for comparison: '{}'.", CSimConfig::jsonToString(json));
          mValid = false;
        }
    }

  if (mLeft.getPrerequisite() != NULL)
    RequiredComputables.insert(mLeft.getPrerequisite());

  if (mRight.getPrerequisite() != NULL)
    RequiredComputables.insert(mRight.getPrerequisite());

  return true;
}

bool CConditionDefinition::isTrue() const
{
  switch (mType)
    {
    case BooleanOperationType::And:
    case BooleanOperationType::Or:
    case BooleanOperationType::Not:
      {
        std::vector< bool > Vector;
        std::vector< CConditionDefinition >::const_iterator it = mBooleanValues.begin();
        std::vector< CConditionDefinition >::const_iterator end = mBooleanValues.end();

        for (; it != end; ++it)
          Vector.push_back(it->isTrue());

        return CCondition::isTrue(mType, Vector);
      }
      break;

    case BooleanOperationType::Value:
      return CCondition::isTrue(mValue);
      break;

    case BooleanOperationType::Comparison:
      {
        if (mComparison != ComparisonType::Within
            && mComparison != ComparisonType::NotWithin)
          return CCondition::isTrue(mComparison, mLeft.value(), mRight.value());
        else
          return CCondition::isTrue(mComparison, mLeft.value(), mRight.valueList());
      }
      break;

    case BooleanOperationType::__SIZE:
      break;
    }

  return true;
}

bool CConditionDefinition::isTrue(const CNode * pNode) const
{
  switch (mType)
    {
    case BooleanOperationType::And:
    case BooleanOperationType::Or:
    case BooleanOperationType::Not:
      {
        std::vector< bool > Vector;
        std::vector< CConditionDefinition >::const_iterator it = mBooleanValues.begin();
        std::vector< CConditionDefinition >::const_iterator end = mBooleanValues.end();

        for (; it != end; ++it)
          Vector.push_back(it->isTrue(pNode));

        return CCondition::isTrue(mType, Vector);
      }
      break;

    case BooleanOperationType::Value:
      return CCondition::isTrue(mValue);
      break;

    case BooleanOperationType::Comparison:
      {
        if (mComparison != ComparisonType::Within
            && mComparison != ComparisonType::NotWithin)
          return CCondition::isTrue(mComparison, mLeft.value(pNode), mRight.value(pNode));
        else
          return CCondition::isTrue(mComparison, mLeft.value(pNode), mRight.valueList());
      }
      break;

    case BooleanOperationType::__SIZE:
      break;
    }

  return true;
}

bool CConditionDefinition::isTrue(const CEdge * pEdge) const
{
  switch (mType)
    {
    case BooleanOperationType::And:
    case BooleanOperationType::Or:
    case BooleanOperationType::Not:
      {
        std::vector< bool > Vector;
        std::vector< CConditionDefinition >::const_iterator it = mBooleanValues.begin();
        std::vector< CConditionDefinition >::const_iterator end = mBooleanValues.end();

        for (; it != end; ++it)
          Vector.push_back(it->isTrue(pEdge));

        return CCondition::isTrue(mType, Vector);
      }
      break;

    case BooleanOperationType::Value:
      return CCondition::isTrue(mValue);
      break;

    case BooleanOperationType::Comparison:
      {
        if (mComparison != ComparisonType::Within
            && mComparison != ComparisonType::NotWithin)
          return CCondition::isTrue(mComparison, mLeft.value(pEdge), mRight.value(pEdge));
        else
          return CCondition::isTrue(mComparison, mLeft.value(pEdge), mRight.valueList());
      }
      break;

    case BooleanOperationType::__SIZE:
      break;
    }

  return true;
}

const bool & CConditionDefinition::isValid() const
{
  return mValid;
}

