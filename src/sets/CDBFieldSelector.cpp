// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2024 Rector and Visitors of the University of Virginia 
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

#include "sets/CDBFieldSelector.h"
#include "db/CConnection.h"
#include "db/CSchema.h"
#include "db/CQuery.h"
#include "db/CFieldValue.h"
#include "db/CFieldValueList.h"
#include "network/CNetwork.h"
#include "network/CNode.h"
#include "utilities/CLogger.h"

CDBFieldSelector::CDBFieldSelector()
  : CSetContent(CSetContent::Type::dbFieldSelector)
  , mTable()
  , mField()
  , mFieldType()
  , mpSelector(NULL)
{}

CDBFieldSelector::CDBFieldSelector(const CDBFieldSelector & src)
  : CSetContent(src)
  , mTable(src.mTable)
  , mField(src.mField)
  , mFieldType(src.mFieldType)
  , mpSelector(src.mpSelector)
{}

CDBFieldSelector::CDBFieldSelector(const json_t * json)
  : CSetContent(CSetContent::Type::dbFieldSelector)
  , mTable()
  , mField()
  , mFieldType()
  , mpSelector(NULL)
{
  fromJSONProtected(json);
}

CDBFieldSelector::~CDBFieldSelector()
{}

// virtual
void CDBFieldSelector::fromJSONProtected(const json_t * json)
{
  /*
    "dbFieldValueSelector": {
      "$id": "#dbFieldValueSelector",
      "description": "A filter returning a list dbFieldValues from the external person trait database.",
      "type": "object",
      "required": [
        "elementType",
        "table",
        "field",
        "selector"
      ],
      "properties": {
        "elementType": {
          "type": "string",
          "enum": ["dbField"]
        },
        "table": {"$ref": "#/definitions/uniqueIdRef"},
        "field": {"$ref": "#/definitions/uniqueIdRef"},
        "selector": {"$ref": "#/definitions/setContent"}
      }
    },
  */

  mValid = false; // DONE
  json_t * pValue = json_object_get(json, "elementType");

  if ((json_is_string(pValue)
       && strcmp(json_string_value(pValue), "dbField") != 0))
    {
      CLogger::error("Field selector: Invalid value for 'elementType.'");
      return;
    }

  // Tables are optional
  pValue = json_object_get(json, "table");

  if (json_is_string(pValue))
    {
      mTable = json_string_value(pValue);
    }

  pValue = json_object_get(json, "field");

  if (json_is_string(pValue))
    {
      mField = json_string_value(pValue);

      if (mTable.empty())
        {
          mTable = CSchema::INSTANCE.getTableForField(mField);
        }

      if (!CSchema::INSTANCE.getTable(mTable).isValid())
        {
          CLogger::error("Field selector: Invalid value for 'table'.");
          return;
        }

      const CField & Field = CSchema::INSTANCE.getTable(mTable).getField(mField);

      if (!Field.isValid())
        {
          CLogger::error("Field selector: Invalid value for 'field'.");
          return;
        }

      mFieldType = Field.getType();
    }
  else
    {
      CLogger::error("Field selector: Invalid or missing value for 'field'.");
      return;
    }

  pValue = json_object_get(json, "selector");

  if (json_is_object(pValue))
    {
      mpSelector = CSetContent::create(pValue);

      if (mpSelector != NULL
          && mpSelector->isValid())
        {
          mPrerequisites.insert(mpSelector.get()); // DONE
        }
      else
        {
          mpSelector.reset();

          {
            CLogger::error("Field selector: Invalid value for 'selector'.");
            return;
          }
        }
    }
  else
    {
      CLogger::error("Field selector: Invalid or missing value for 'selector'.");
      return;
    }

  CConnection::setRequired(true);
  mValid = true;
}

// virtual 
bool CDBFieldSelector::lessThanProtected(const CSetContent & rhs) const
{
  const CDBFieldSelector * pRhs = static_cast< const CDBFieldSelector * >(&rhs);

  if (mTable != pRhs->mTable)  
    return mTable < pRhs->mTable;

  if (mField != pRhs->mField)  
    return mField < pRhs->mField;
    
  if (mFieldType != pRhs->mFieldType)  
    return mFieldType < pRhs->mFieldType;
    
  return mpSelector < pRhs->mpSelector;
}

// virtual
bool CDBFieldSelector::computeSetContent()
{
  SetContent & Content = activeContent();

  CDBFieldValues & DBFieldValues = Content.dBFieldValues;

  CFieldValueList FieldValueList;
  CFieldValueList ConstraintValueList;

  std::vector< CNode * >::const_iterator itConstraint = mpSelector->activeContent().nodes(mScope).begin();
  std::vector< CNode * >::const_iterator endConstraint = mpSelector->activeContent().nodes(mScope).end();

  for (; itConstraint != endConstraint; ++itConstraint)
    {
      ConstraintValueList.append(CFieldValue((*itConstraint)->id));
    }

  bool success = CQuery::in(mTable, mField, FieldValueList, false, "pid", ConstraintValueList);

  if (FieldValueList.size() > 0)
    {
      DBFieldValues.emplace(FieldValueList.getType(), FieldValueList);
    }

  // std::cout << "CDBFieldSelector::compute: " << FieldValueList.size() << std::endl;
  return success;
}

// virtual 
void CDBFieldSelector::setScopeProtected()
{
  if (mpSelector)
    mpSelector->setScope(Scope::global);
}

