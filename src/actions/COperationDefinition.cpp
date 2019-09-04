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

#include <cstring>
#include <jansson.h>

#include "actions/COperationDefinition.h"
#include "actions/COperation.h"
#include "math/CNodeProperty.h"
#include "math/CEdgeProperty.h"
#include "variables/CVariableList.h"

COperationDefinition::COperationDefinition()
  : CAnnotation()
  , mTargetType()
  , mpNodeProperty(NULL)
  , mpEdgeProperty(NULL)
  , mpVariable(NULL)
  , mpOperator(NULL)
  , mValue((json_t *) NULL)
  , mValid(false)
{}

COperationDefinition::COperationDefinition(const COperationDefinition & src)
  : CAnnotation(src)
  , mTargetType()
  , mpNodeProperty(src.mpNodeProperty != NULL ? new CNodeProperty(*src.mpNodeProperty) : NULL)
  , mpEdgeProperty(src.mpEdgeProperty != NULL ? new CEdgeProperty(*src.mpEdgeProperty) : NULL)
  , mpVariable(src.mpVariable)
  , mpOperator(src.mpOperator)
  , mValue(src.mValue)
  , mValid(src.mValid)
{}

COperationDefinition::COperationDefinition(const json_t * json)
  : CAnnotation()
  , mTargetType()
  , mpNodeProperty(NULL)
  , mpEdgeProperty(NULL)
  , mpVariable(NULL)
  , mpOperator(NULL)
  , mValue((json_t *) NULL)
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
      {"$ref": "#/definitions/value"}
    ]
  */

  json_t * pValue = json_object_get(json, "target");

  if (!json_is_object(pValue))
    {
      mValid = false;
      return;
    }

  mpVariable = & CVariableList::INSTANCE[pValue];

  if (mpVariable->isValid())
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
              mValid = false;
              return;
            }
        }
    }

  pValue = json_object_get(json, "operator");

  if (!json_is_string(pValue))
    {
      mValid = false;
      return;
    }

  if (strcmp(json_string_value(pValue), "=") == 0)
    {
      mpOperator = &CValueInterface::equal;
    }
  else if(strcmp(json_string_value(pValue), "+=") == 0)
    {
      mpOperator = &CValueInterface::plus;
    }
  else if(strcmp(json_string_value(pValue), "-=") == 0)
    {
      mpOperator = &CValueInterface::minus;
    }
  else if(strcmp(json_string_value(pValue), "*=") == 0)
    {
      mpOperator = &CValueInterface::multiply;
    }
  else if(strcmp(json_string_value(pValue), "/=") == 0)
    {
      mpOperator = &CValueInterface::divide;
    }
  else if(strcmp(json_string_value(pValue), ">") == 0)
    {
      mValid = false;
      return;
    }

  mValue.fromJSON(json);

  mValid = mValue.isValid();
}

const bool & COperationDefinition::isValid() const
{
  return mValid;
}

COperation * COperationDefinition::createOperation(CNode * pNode) const
{
  return mpNodeProperty->createOperation(pNode, mValue, mpOperator);
}

COperation * COperationDefinition::createOperation(CEdge * pEdge) const
{
  return mpEdgeProperty->createOperation(pEdge, mValue, mpOperator);
}

COperation * COperationDefinition::createOperation(CVariable * pVariable) const
{
  return new COperationInstance< CVariable, double >(pVariable, mValue.toNumber(), mpOperator, &CVariable::setValue);
}