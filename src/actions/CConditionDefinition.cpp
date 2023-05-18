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

CConditionDefinition::ValueInstance::ValueInstance()
  : type()
  , pValue(NULL)
  , pValueList(NULL)
  , pObservable(NULL)
  , pNodeProperty(NULL)
  , pEdgeProperty(NULL)
  , pVariable(NULL)
  , pSizeOf(NULL)
  , valid(false)
{}

CConditionDefinition::ValueInstance::ValueInstance(const CConditionDefinition::ValueInstance & src)
  : type(src.type)
  , pValue(src.pValue)
  , pValueList(src.pValueList)
  , pObservable(src.pObservable)
  , pNodeProperty(src.pNodeProperty)
  , pEdgeProperty(src.pEdgeProperty)
  , pVariable(src.pVariable)
  , pSizeOf(src.pSizeOf)
  , valid(src.valid)
{}

CConditionDefinition::ValueInstance::~ValueInstance()
{}

void CConditionDefinition::ValueInstance::fromJSON(const json_t * json)
{
  valid = false; //DONE

  if (!json_is_object(json))
    {
      CLogger::error("Value Instance: Invalid.");
      return;
    }

  CLogger::pushLevel(spdlog::level::off);

  pValue = std::shared_ptr< CValue >(new CValue(json));

  if (pValue->isValid())
    {
      type = ValueType::Value;
      valid = true;
      goto final;
    }

  pValue.reset();

  pValueList = std::shared_ptr< CValueList >(new CValueList(json));

  if (pValueList->isValid())
    {
      type = ValueType::ValueList;
      valid = true;
      goto final;
    }

  pValueList.reset();

  pObservable = CObservable::get(json);

  if (pObservable
      && pObservable->isValid())
    {
      type = ValueType::Observable;
      RequiredComputables.insert(pObservable);
      valid = true;
      goto final;
    }

  pObservable = NULL;

  pNodeProperty = std::shared_ptr< CNodeProperty >(new CNodeProperty(json));

  if (pNodeProperty->isValid())
    {
      type = ValueType::NodeProperty;
      valid = true;
      goto final;
    }

  pNodeProperty.reset();

  pEdgeProperty = std::shared_ptr< CEdgeProperty >(new CEdgeProperty(json));

  if (pEdgeProperty->isValid())
    {
      type = ValueType::EdgeProperty;
      valid = true;
      goto final;
    }

  pEdgeProperty.reset();

  pVariable = &CVariableList::INSTANCE[json];

  if (pVariable->isValid())
    {
      type = ValueType::Variable;
      valid = true;
      goto final;
    }

  pVariable = NULL;

  pSizeOf = std::shared_ptr< CSizeOf >(new CSizeOf(json));

  if (pSizeOf->isValid())
    {
      type = ValueType::Sizeof;
      RequiredComputables.insert(pSizeOf.get());
      valid = true;
      goto final;
    }

  pSizeOf.reset();

final:
  CLogger::popLevel();

  if (!valid)
    {
      CLogger::error() << "Value Instance: Invalid. " << CSimConfig::jsonToString(json);
    }
}

CValueInterface * CConditionDefinition::ValueInstance::value(const CNode * pNode) const
{
  if (pNodeProperty)
    return pNodeProperty->propertyOf(pNode).copy();

  return value();
}

CValueInterface * CConditionDefinition::ValueInstance::value(const CEdge * pEdge) const
{
  if (pEdgeProperty)
    return pEdgeProperty->propertyOf(pEdge).copy();

  return value();
}

CValueInterface * CConditionDefinition::ValueInstance::value() const
{
  switch (type)
    {
    case ValueType::Value:
      return pValue.get();
      break;

    case ValueType::Observable:
      return pObservable;
      break;

    case ValueType::Variable:
      return pVariable;
      break;

    case ValueType::Sizeof:
      return pSizeOf.get();
      break;

    default:
      break;
    }

  // pValueList is only available for Within and NotWithin which does not use this path

  return NULL;
}

CValueInterface::Type CConditionDefinition::ValueInstance::interfaceType() const
{
  switch (type)
    {
    case ValueType::Value:
      if (pValue)
        return pValue->getType();

      break;

    case ValueType::ValueList:
      if (pValueList)
        return pValueList->getType();

      break;

    case ValueType::Observable:
      if (pObservable)
        return pObservable->getType();

      break;

    case ValueType::NodeProperty:
      if (pNodeProperty)
        return pNodeProperty->getType();

      break;

    case ValueType::EdgeProperty:
      if (pEdgeProperty)
        return pEdgeProperty->getType();

      break;

    case ValueType::Variable:
      if (pVariable)
        return pVariable->getType();

      break;

    case ValueType::Sizeof:
      if (pSizeOf)
        return pSizeOf->getType();

      break;
    }

  return CValueList::Type::boolean;
}

bool CConditionDefinition::ValueInstance::inherit() const
{
  return pNodeProperty || pEdgeProperty;
}

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
  : mType()
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

  CLogger::error() << "Condition: Invalid. " << CSimConfig::jsonToString(json);
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

  mValid = false; // DONE
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

  mValid = false; // DONE
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
          CLogger::error() << "Condition: Invalid operant for Boolean operator '" << (mType == BooleanOperationType::And ? "and" : "or") << "'.";
          mValid = false; // DONE
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
  mValid = false; // DONE

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
      CLogger::error() << "Condition: Invalid comparison operator '" << CSimConfig::jsonToString(json) << "'.";
      return true;
    }

  mValid = true;

  mLeft.fromJSON(json_object_get(json, "left"));

  if (!mLeft.valid)
    {
      CLogger::error() << "Condition: Invalid left operant for comparison operator '" << CSimConfig::jsonToString(json) << "'.";
      mValid = false; // DONE
    }

  mRight.fromJSON(json_object_get(json, "right"));

  if (!mRight.valid)
    {
      CLogger::error() << "Condition: Invalid right operant for comparison operator '" << CSimConfig::jsonToString(json) << "'.";
      mValid = false; // DONE
    }

  if (mComparison != ComparisonType::Within
      && mComparison != ComparisonType::NotWithin)
    {
      if (mLeft.type == ValueType::ValueList)
        {
          CLogger::error() << "Condition: Invalid value type for left operant for comparison operator '" << CSimConfig::jsonToString(json) << "'.";
          mValid = false; // DONE
        }

      if (mRight.type == ValueType::ValueList)
        {
          CLogger::error() << "Condition: Invalid value type for right operant for comparison operator '" << CSimConfig::jsonToString(json) << "'.";
          mValid = false; // DONE
        }

      if (!compatible(mLeft, mRight))
        {
          CLogger::error() << "Condition: Incompatible values for comparison: '" << CSimConfig::jsonToString(json) << "'.";
          mValid = false; // DONE
        }
    }
  else
    {
      if (mLeft.type != ValueType::NodeProperty
          && mLeft.type != ValueType::EdgeProperty
          && mLeft.type != ValueType::Observable
          && mLeft.type != ValueType::Variable)
        {
          CLogger::error() << "Condition: Invalid value type for left operant for comparison operator '" << CSimConfig::jsonToString(json) << "'.";
          mValid = false; // DONE
        }

      if (mRight.type != ValueType::ValueList)
        {
          CLogger::error() << "Condition: Invalid value type for right operant for comparison operator '" << CSimConfig::jsonToString(json) << "'.";
          mValid = false; // DONE
        }

      if (!compatible(mLeft, mRight))
        {
          CLogger::error() << "Condition: Incompatible values for comparison: '" << CSimConfig::jsonToString(json) << "'.";
          mValid = false; // DONE
        }
    }

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
          {
            Vector.push_back(it->isTrue());
          }

        return CCondition::isTrue(mType, Vector);
      }
      break;

    case BooleanOperationType::Value:
      return CCondition::isTrue(mValue);
      break;

    case BooleanOperationType::Comparison:
      {
        CValueInterface * pLeft = mLeft.value();

        if (mComparison != ComparisonType::Within
            && mComparison != ComparisonType::NotWithin)
          {
            CValueInterface * pRight = mRight.value();

            if (pLeft != NULL
                && pRight != NULL)
              return CCondition::isTrue(mComparison, pLeft, pRight);
          }
        else if (mRight.pValueList != NULL
                 && pLeft != NULL)
          {
            return CCondition::isTrue(mComparison, pLeft, *mRight.pValueList);
          }
      }
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
          {
            Vector.push_back(it->isTrue(pNode));
          }

        return CCondition::isTrue(mType, Vector);
      }
      break;

    case BooleanOperationType::Value:
      return CCondition::isTrue(mValue);
      break;

    case BooleanOperationType::Comparison:
      {
        CValueInterface * pLeft = mLeft.value(pNode);

        if (mComparison != ComparisonType::Within
            && mComparison != ComparisonType::NotWithin)
          {
            CValueInterface * pRight = mRight.value(pNode);

            if (pLeft != NULL
                && pRight != NULL)
              return CCondition::isTrue(mComparison, pLeft, pRight);
          }
        else if (mRight.pValueList != NULL
                 && pLeft != NULL)
          {
            return CCondition::isTrue(mComparison, pLeft, *mRight.pValueList);
          }
      }
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
          {
            Vector.push_back(it->isTrue(pEdge));
          }

        return CCondition::isTrue(mType, Vector);
      }
      break;

    case BooleanOperationType::Value:
      return CCondition::isTrue(mValue);
      break;

    case BooleanOperationType::Comparison:
      {
        CValueInterface * pLeft = mLeft.value(pEdge);

        if (mComparison != ComparisonType::Within
            && mComparison != ComparisonType::NotWithin)
          {
            CValueInterface * pRight = mRight.value(pEdge);

            if (pLeft != NULL
                && pRight != NULL)
              return CCondition::isTrue(mComparison, pLeft, pRight);
          }
        else if (mRight.pValueList != NULL
                 && pLeft != NULL)
          {
            return CCondition::isTrue(mComparison, pLeft, *mRight.pValueList);
          }
      }
      break;
    }

  return true;
}

const bool & CConditionDefinition::isValid() const
{
  return mValid;
}

// static
bool CConditionDefinition::compatible(const CConditionDefinition::ValueInstance & lhs, const CConditionDefinition::ValueInstance & rhs)
{
  return CValueInterface::compatible(lhs.interfaceType(), rhs.interfaceType());
}
