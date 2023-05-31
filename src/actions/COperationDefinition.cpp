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

#include "actions/COperationDefinition.h"
#include "actions/COperation.h"
#include "actions/CConditionDefinition.h"
#include "math/CNodeProperty.h"
#include "math/CEdgeProperty.h"
#include "math/CObservable.h"
#include "math/CSizeOf.h"
#include "variables/CVariable.h"
#include "variables/CVariableList.h"
#include "utilities/CMetadata.h"
#include "utilities/CLogger.h"
#include "utilities/CSimConfig.h"

COperationDefinition::COperationDefinition()
  : CAnnotation()
  , mTarget()
  , mpOperator(NULL)
  , mSource()
  , mValid(false)
{}

COperationDefinition::COperationDefinition(const COperationDefinition & src)
  : CAnnotation(src)
  , mTarget(src.mTarget)
  , mpOperator(src.mpOperator)
  , mSource(src.mSource)
  , mValid(src.mValid)
{}

COperationDefinition::COperationDefinition(const json_t * json)
  : CAnnotation()
  , mTarget()
  , mpOperator(NULL)
  , mSource()
  , mValid(false)
{
  fromJSON(json);
}

// virtual
COperationDefinition::~COperationDefinition()
{}

void COperationDefinition::fromJSON(const json_t * json)
{
  /*
    "allOf": [
      {"$ref": "#/definitions/annotation"},
      {
        "description": "",
        "type": "object",
        "required": ["target"],
        "properties": {
          "target": {
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
              },
              {"$ref": "#/definitions/variableReference"}
            ]
          }
        }
      },
      {
        "description": "",
        "type": "object",
        "required": ["operator"],
        "properties": {
          "operator": {
            "type": "string",
            "enum": [
              "=",
              "*=",
              "/=",
              "+=",
              "-="
            ]
          }
        }
      },
      {
        "oneOf": [
          {"$ref": "#/definitions/value"},
          {"$ref": "#/definitions/observable"},
          {"$ref": "#/definitions/variableReference"},
          {"$ref": "#/definitions/sizeof"}
        ]
      }
    ]
  */

  mValid = false; // DONE
  json_t * pValue = json_object_get(json, "target");

  mTarget.fromJSON(pValue, true);
   
  if (!mTarget.isValid())
    {
              CLogger::error() << "Operation: Invalid target '" << CSimConfig::jsonToString(json) << "'.";
      return;
    }

  pValue = json_object_get(json, "operator");

  if (!json_is_string(pValue))
    {
      CLogger::error() << "Operation: Invalid operator '" << CSimConfig::jsonToString(json) << "'.";
      return;
    }

  const char * Operator = json_string_value(pValue);

  if (strcmp(Operator, "=") == 0)
    {
      mpOperator = &CValueInterface::equal;
    }
  else if (strcmp(Operator, "+=") == 0)
    {
      mpOperator = &CValueInterface::plus;
    }
  else if (strcmp(Operator, "-=") == 0)
    {
      mpOperator = &CValueInterface::minus;
    }
  else if (strcmp(Operator, "*=") == 0)
    {
      mpOperator = &CValueInterface::multiply;
    }
  else if (strcmp(Operator, "/=") == 0)
    {
      mpOperator = &CValueInterface::divide;
    }
  else
    {
      CLogger::error() << "Operation: Invalid operator '" << CSimConfig::jsonToString(json) << "'.";
    }

  if (mpOperator != &CValueInterface::equal
      && mTarget.interfaceType() != CValueInterface::Type::number 
      && mTarget.interfaceType() != CValueInterface::Type::integer)
    {
      CLogger::error() << "Operation: Target value type must be numeric for operator '" << CSimConfig::jsonToString(json) << "'.";
      return;
    }

  mSource.fromJSON(json, false);

  if (!mSource.isValid())
    {
      CLogger::error("Operation: Invalid source.");
      return;
    }

  if (mSource.getPrerequisite() != NULL)
    CConditionDefinition::RequiredComputables.insert(mSource.getPrerequisite());

  if (!CValueInterface::compatible(mTarget.interfaceType(), mSource.interfaceType()))
    {
      CLogger::error() << "Operation: Incompatible value types for '" << CSimConfig::jsonToString(json) << "'.";
      return;
    }

  mValid = true;
}

const bool & COperationDefinition::isValid() const
{
  return mValid;
}

bool COperationDefinition::execute(CNode * pNode, const CMetadata & info) const
{
  CNodeProperty * pProperty = mTarget.nodeProperty();

  if (pNode == NULL
      || pProperty == NULL)
    return execute(info);

  switch (mSource.getType())
    {
    case CValueInstance::ValueType::Variable:
    case CValueInstance::ValueType::Observable:
    case CValueInstance::ValueType::SizeOf:
    case CValueInstance::ValueType::Value:
      return pProperty->execute(pNode, mSource.value(), mpOperator, info);
      break;

    default:
      break;
    }

  return false;
}

bool COperationDefinition::execute(CEdge * pEdge, const CMetadata & info) const
{
  CEdgeProperty * pProperty = mTarget.edgeProperty();

  if (pEdge == NULL
      || pProperty == NULL)
    return execute(info);

  switch (mSource.getType())
    {
    case CValueInstance::ValueType::Variable:
    case CValueInstance::ValueType::Observable:
    case CValueInstance::ValueType::SizeOf:
    case CValueInstance::ValueType::Value:
      return pProperty->execute(pEdge, mSource.value(), mpOperator, info);
      break;


    default:
      break;
    }

  return false;
}

bool COperationDefinition::execute(const CMetadata & info) const
{
  CVariable * pVariable = mTarget.variable();

  if (pVariable == NULL)
    return false;

  switch (mSource.getType())
    {
    case CValueInstance::ValueType::Variable:
    case CValueInstance::ValueType::Observable:
    case CValueInstance::ValueType::SizeOf:
    case CValueInstance::ValueType::Value:
      return COperation::execute< CVariable, CValueInterface >(pVariable, mSource.value(), mpOperator, &CVariable::setValue, info);
      break;

    default:
      break;
    }

  return false;
}
