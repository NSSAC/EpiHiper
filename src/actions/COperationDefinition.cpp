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

#include <cstring>
#include <jansson.h>

#include "actions/COperationDefinition.h"
#include "actions/COperation.h"
#include "actions/CConditionDefinition.h"
#include "math/CNodeProperty.h"
#include "math/CEdgeProperty.h"
#include "math/CObservable.h"
#include "math/CSizeOf.h"
#include "variables/CVariableList.h"
#include "utilities/CMetadata.h"
#include "utilities/CLogger.h"

COperationDefinition::COperationDefinition()
  : CAnnotation()
  , mTargetType()
  , mpNodeProperty(NULL)
  , mpEdgeProperty(NULL)
  , mpTargetVariable(NULL)
  , mpOperator(NULL)
  , mpSourceVariable(NULL)
  , mpObservable(NULL)
  , mpSizeOf(NULL)
  , mValue((json_t *) NULL)
  , mValid(false)
{}

COperationDefinition::COperationDefinition(const COperationDefinition & src)
  : CAnnotation(src)
  , mTargetType()
  , mpNodeProperty(src.mpNodeProperty != NULL ? new CNodeProperty(*src.mpNodeProperty) : NULL)
  , mpEdgeProperty(src.mpEdgeProperty != NULL ? new CEdgeProperty(*src.mpEdgeProperty) : NULL)
  , mpTargetVariable(src.mpTargetVariable)
  , mpOperator(src.mpOperator)
  , mpSourceVariable(src.mpSourceVariable)
  , mpObservable(src.mpObservable)
  , mpSizeOf(src.mpSizeOf)
  , mValue(src.mValue)
  , mValid(src.mValid)
{}

COperationDefinition::COperationDefinition(const json_t * json)
  : CAnnotation()
  , mTargetType()
  , mpNodeProperty(NULL)
  , mpEdgeProperty(NULL)
  , mpTargetVariable(NULL)
  , mpOperator(NULL)
  , mpSourceVariable(NULL)
  , mpObservable(NULL)
  , mpSizeOf(NULL)
  , mValue((json_t *) NULL)
  , mValid(false)
{
  fromJSON(json);
}

// virtual
COperationDefinition::~COperationDefinition()
{
  if (mpNodeProperty != NULL)
    delete mpNodeProperty;
  if (mpEdgeProperty != NULL)
    delete mpEdgeProperty;
}

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

  char * str = json_dumps(json, JSON_COMPACT | JSON_INDENT(0));
  std::string JSON = str;
  free(str);

  mValid = false; // DONE
  json_t * pValue = json_object_get(json, "target");

  if (!json_is_object(pValue))
    {
      CLogger::error("Operation: Invalid target.");
      return;
    }

  mpTargetVariable = &CVariableList::INSTANCE[pValue];

  if (mpTargetVariable->isValid())
    {
      mTargetType = TargetType::variable;
    }
  else
    {
      CNodeProperty NodeProperty(pValue);

      if (NodeProperty.isValid())
        {
          mpNodeProperty = new CNodeProperty(NodeProperty);
          mTargetType = TargetType::node;
        }
      else
        {
          CEdgeProperty EdgeProperty(pValue);

          if (EdgeProperty.isValid())
            {
              mpEdgeProperty = new CEdgeProperty(EdgeProperty);
              mTargetType = TargetType::edge;
            }
          else
            {
              CLogger::error("Operation: Invalid target.");
              return;
            }
        }
    }

  pValue = json_object_get(json, "operator");

  if (!json_is_string(pValue))
    {
      CLogger::error("Operation: Invalid operator.");
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
      CLogger::error() << "Operation: Invalid operator '" << Operator << "'.";
      return;
    }

  mpSourceVariable = &CVariableList::INSTANCE[json];

  if (mpSourceVariable->isValid())
    {
      mValid = true;
      return; 
    }

  mpSourceVariable = NULL;

  mpObservable = CObservable::get(json);

  if (mpObservable != NULL
      && mpObservable->isValid())
    {
      CConditionDefinition::RequiredComputables.insert(mpObservable);
      mValid = true;
      return;
    }

  mpObservable = NULL;

  CLogger::pushLevel(spdlog::level::off);
  mpSizeOf = new CSizeOf(json);
  CLogger::popLevel();
  
  if (mpSizeOf->isValid())
    {
      CConditionDefinition::RequiredComputables.insert(mpSizeOf);
      mValid = true;
      return;
    }

  delete mpSizeOf;
  mpSizeOf = NULL;

  mValue.fromJSON(json_object_get(json, "value"));

  if (!mValue.isValid())
    {
      char * str = json_dumps(json, JSON_COMPACT | JSON_INDENT(0));
      std::string JSON = str;
      free(str);

      CLogger::error() << "Operation: Invalid '" << JSON;
    }

  mValid = mValue.isValid();
}

const bool & COperationDefinition::isValid() const
{
  return mValid;
}

COperation * COperationDefinition::createOperation(CNode * pNode, const CMetadata & info) const
{
  if (pNode == NULL
      || mpNodeProperty == NULL)
    return createOperation(info);

  if (mpSourceVariable != NULL)
    return mpNodeProperty->createOperation(pNode, *mpSourceVariable, mpOperator, info);

  if (mpObservable != NULL)
    return mpNodeProperty->createOperation(pNode, *mpObservable, mpOperator, info);

  if (mpSizeOf != NULL)
    return mpNodeProperty->createOperation(pNode, *mpSizeOf, mpOperator, info);

  return mpNodeProperty->createOperation(pNode, mValue, mpOperator, info);
}

COperation * COperationDefinition::createOperation(CEdge * pEdge, const CMetadata & info) const
{
  if (pEdge == NULL
      || mpEdgeProperty == NULL)
    return createOperation(info);

  if (mpSourceVariable != NULL)
    return mpEdgeProperty->createOperation(pEdge, *mpSourceVariable, mpOperator, info);

  if (mpObservable != NULL)
    return mpEdgeProperty->createOperation(pEdge, *mpObservable, mpOperator, info);

  if (mpSizeOf != NULL)
    return mpEdgeProperty->createOperation(pEdge, *mpSizeOf, mpOperator, info);

  return mpEdgeProperty->createOperation(pEdge, mValue, mpOperator, info);
}

COperation * COperationDefinition::createOperation(const CMetadata & info) const
{
  if (mpSourceVariable != NULL)
    return new COperationInstance< CVariable, CValue >(mpTargetVariable, *mpSourceVariable, mpOperator, &CVariable::setValue, info);

  if (mpObservable != NULL)
    return new COperationInstance< CVariable, CValue >(mpTargetVariable, *mpObservable, mpOperator, &CVariable::setValue, info);

  if (mpSizeOf != NULL)
    return new COperationInstance< CVariable, CValue >(mpTargetVariable, *mpSizeOf, mpOperator, &CVariable::setValue, info);

  return new COperationInstance< CVariable, CValue >(mpTargetVariable, mValue, mpOperator, &CVariable::setValue, info);
}
