// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

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

CConditionDefinition::ValueInstance::ValueInstance()
  : pCounter(new size_t(1))
  , type()
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
  : pCounter(src.pCounter)
  , type(src.type)
  , pValue(src.pValue)
  , pValueList(src.pValueList)
  , pObservable(src.pObservable)
  , pNodeProperty(src.pNodeProperty)
  , pEdgeProperty(src.pEdgeProperty)
  , pVariable(src.pVariable)
  , pSizeOf(src.pSizeOf)
  , valid(src.valid)
{
  ++(*pCounter);
}

CConditionDefinition::ValueInstance::~ValueInstance()
{
  (*pCounter)--;

  if (*pCounter == 0)
    {
      delete pCounter;
      if (pValue != NULL)
        delete pValue;
      if (pValueList != NULL)
        delete pValueList;
      if (pNodeProperty != NULL)
        delete pNodeProperty;
      if (pEdgeProperty != NULL)
        delete pEdgeProperty;
      if (pSizeOf != NULL)
        delete pSizeOf;
    }
}

void CConditionDefinition::ValueInstance::fromJSON(const json_t * json)
{
  valid = false; //DONE

  if (!json_is_object(json))
    {
      CLogger::error("Value Instance: Invalid.");
      return;
    }

  CLogger::pushLevel(spdlog::level::off);
  pValue = new CValue(json);

  if (pValue->isValid())
    {
      type = ValueType::Value;
      valid = true;
      goto final;
    }

  delete pValue;
  pValue = NULL;

  pValueList = new CValueList(json);

  if (pValueList->isValid())
    {
      type = ValueType::ValueList;
      valid = true;
      goto final;
    }

  delete pValueList;
  pValueList = NULL;

  pObservable = CObservable::get(json);

  if (pObservable != NULL
      && pObservable->isValid())
    {
      type = ValueType::Observable;
      RequiredComputables.insert(pObservable);
      valid = true;
      goto final;
    }

  pObservable = NULL;

  pNodeProperty = new CNodeProperty(json);

  if (pNodeProperty->isValid())
    {
      type = ValueType::NodeProperty;
      valid = true;
      goto final;
    }

  delete pNodeProperty;
  pNodeProperty = NULL;

  pEdgeProperty = new CEdgeProperty(json);

  if (pEdgeProperty->isValid())
    {
      type = ValueType::EdgeProperty;
      valid = true;
      goto final;
    }

  delete pEdgeProperty;
  pEdgeProperty = NULL;

  pVariable = &CVariableList::INSTANCE[json];

  if (pVariable->isValid())
    {
      type = ValueType::Variable;
      valid = true;
      goto final;
    }

  pVariable = NULL;

  pSizeOf = new CSizeOf(json);

  if (pSizeOf->isValid())
    {
      type = ValueType::Sizeof;
      RequiredComputables.insert(pSizeOf);
      valid = true;
      goto final;
    }

  delete pSizeOf;
  pSizeOf = NULL;

final:
  CLogger::popLevel();

  if (!valid)
    CLogger::error("Value Instance: Invalid.");
}

CValueInterface * CConditionDefinition::ValueInstance::value(const CNode * pNode) const
{
  if (pNodeProperty != NULL)
    return &pNodeProperty->propertyOf(pNode);

  return value();
}

CValueInterface * CConditionDefinition::ValueInstance::value(const CEdge * pEdge) const
{
  if (pEdgeProperty != NULL)
    return &pEdgeProperty->propertyOf(pEdge);

  return value();
}

CValueInterface * CConditionDefinition::ValueInstance::value() const
{
  if (pValue != NULL)
    return pValue;

  if (pObservable != NULL)
    return pObservable;

  if (pVariable != NULL)
    return pVariable;

  if (pSizeOf != NULL)
    return pSizeOf;

  // pValueList is only available for Within and NotWithin which does not use this path

  return NULL;
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

  if (valueFromJSON(json))
    return;

  if (comparisonFromJSON(json))
    return;

  if (operationFromJSON(json)) // also handles not
    return;

  mType = BooleanOperationType::Value;
  mValue = true;
  mValid = true;
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
      CLogger::error() << "Condition: Invalid comparison operator '" << Comparison << "'.";
      return true;
    }

  mValid = true;

  mLeft.fromJSON(json_object_get(json, "left"));

  if (!mLeft.valid)
    {
      CLogger::error() << "Condition: Invalid left operant for comparison operator '" << Comparison << "'.";
      mValid = false; // DONE
    }

  mRight.fromJSON(json_object_get(json, "right"));

  if (!mRight.valid)
    {
      CLogger::error() << "Condition: Invalid right operant for comparison operator '" << Comparison << "'.";
      mValid = false; // DONE
    }

  if (mComparison != ComparisonType::Within
      && mComparison != ComparisonType::NotWithin)
    {
      if (mLeft.type == ValueType::ValueList)
        {
          CLogger::error() << "Condition: Invalid value type for left operant for comparison operator '" << Comparison << "'.";
          mValid = false; // DONE
        }

      if (mRight.type == ValueType::ValueList)
        {
          CLogger::error() << "Condition: Invalid value type for right operant for comparison operator '" << Comparison << "'.";
          mValid = false; // DONE
        }
    }
  else
    {
      if (mLeft.type != ValueType::NodeProperty
          && mLeft.type != ValueType::EdgeProperty)
        {
          CLogger::error() << "Condition: Invalid value type for left operant for comparison operator '" << Comparison << "'.";
          mValid = false; // DONE
        }

      if (mRight.type != ValueType::ValueList)
        {
          CLogger::error() << "Condition: Invalid value type for right operant for comparison operator '" << Comparison << "'.";
          mValid = false; // DONE
        }
    }

  return true;
}

CCondition * CConditionDefinition::createCondition() const
{
  switch (mType)
    {
    case BooleanOperationType::And:
    case BooleanOperationType::Or:
    case BooleanOperationType::Not:
      {
        std::vector< CCondition * > Vector;
        std::vector< CConditionDefinition >::const_iterator it = mBooleanValues.begin();
        std::vector< CConditionDefinition >::const_iterator end = mBooleanValues.end();

        for (; it != end; ++it)
          {
            Vector.push_back(it->createCondition());
          }

        return new CBooleanOperation(mType, Vector);
      }
      break;

    case BooleanOperationType::Value:
      return new CBooleanValue(mValue);
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
              return new CComparison(mComparison, pLeft, pRight);
          }
        else if (mRight.pValueList != NULL
                 && pLeft != NULL)
          {
            return new CContainedIn(mComparison, pLeft, *mRight.pValueList);
          }
      }
      break;
    }

  return new CBooleanValue(true);
}

CCondition * CConditionDefinition::createCondition(const CNode * pNode) const
{
  switch (mType)
    {
    case BooleanOperationType::And:
    case BooleanOperationType::Or:
    case BooleanOperationType::Not:
      {
        std::vector< CCondition * > Vector;
        std::vector< CConditionDefinition >::const_iterator it = mBooleanValues.begin();
        std::vector< CConditionDefinition >::const_iterator end = mBooleanValues.end();

        for (; it != end; ++it)
          {
            Vector.push_back(it->createCondition(pNode));
          }

        return new CBooleanOperation(mType, Vector);
      }
      break;

    case BooleanOperationType::Value:
      return new CBooleanValue(mValue);
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
              return new CComparison(mComparison, pLeft, pRight);
          }
        else if (mRight.pValueList != NULL
                 && pLeft != NULL)
          {
            return new CContainedIn(mComparison, pLeft, *mRight.pValueList);
          }
      }
      break;
    }

  return new CBooleanValue(true);
}

CCondition * CConditionDefinition::createCondition(const CEdge * pEdge) const
{
  switch (mType)
    {
    case BooleanOperationType::And:
    case BooleanOperationType::Or:
    case BooleanOperationType::Not:
      {
        std::vector< CCondition * > Vector;
        std::vector< CConditionDefinition >::const_iterator it = mBooleanValues.begin();
        std::vector< CConditionDefinition >::const_iterator end = mBooleanValues.end();

        for (; it != end; ++it)
          {
            Vector.push_back(it->createCondition(pEdge));
          }

        return new CBooleanOperation(mType, Vector);
      }
      break;

    case BooleanOperationType::Value:
      return new CBooleanValue(mValue);
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
              return new CComparison(mComparison, pLeft, pRight);
          }
        else if (mRight.pValueList != NULL
                 && pLeft != NULL)
          {
            return new CContainedIn(mComparison, pLeft, *mRight.pValueList);
          }
      }
      break;
    }

  return new CBooleanValue(true);
}

const bool & CConditionDefinition::isValid() const
{
  return mValid;
}
