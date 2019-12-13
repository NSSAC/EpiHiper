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

#include "sets/CDBFieldSelector.h"
#include "db/CSchema.h"
#include "db/CQuery.h"
#include "db/CFieldValue.h"
#include "db/CFieldValueList.h"
#include "network/CNode.h"

CDBFieldSelector::CDBFieldSelector()
  : CSetContent()
  , mTable()
  , mField()
  , mFieldType()
  , mpSelector(NULL)
{}

CDBFieldSelector::CDBFieldSelector(const CDBFieldSelector & src)
  : CSetContent(src)
  , mTable()
  , mField()
  , mFieldType(src.mFieldType)
  , mpSelector(CSetContent::copy(src.mpSelector))
{}

CDBFieldSelector::CDBFieldSelector(const json_t * json)
  : CSetContent()
  , mTable()
  , mField()
  , mFieldType()
  , mpSelector(NULL)
{
  fromJSON(json);
}

CDBFieldSelector::~CDBFieldSelector()
{
  CSetContent::destroy(mpSelector);
}

// virtual
void CDBFieldSelector::fromJSON(const json_t * json)
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

  json_t * pValue = json_object_get(json, "elementType");

  mValid = (json_is_string(pValue) && strcmp(json_string_value(pValue), "dbField") == 0);

  if (!mValid) return;

  // Tables are optional
  pValue = json_object_get(json, "table");

  if (json_is_string(pValue))
    {
      mTable = json_string_value(pValue);
      mValid &= CSchema::INSTANCE.getTable(mTable).isValid();
    }

  pValue = json_object_get(json, "field");

  if (json_is_string(pValue))
    {
      mField = json_string_value(pValue);

      if (mTable.empty())
        {
          mTable = CSchema::INSTANCE.getTableForField(mField);
        }

      const CField & Field = CSchema::INSTANCE.getTable(mTable).getField(mField);
      mFieldType = Field.getType();
      mValid &= Field.isValid();
    }
  else
    {
      mValid = false;
    }

  pValue = json_object_get(json, "selector");

  if (json_is_object(pValue))
    {
      mpSelector = CSetContent::create(pValue);

      if (mpSelector != NULL && mpSelector->isValid())
        {
          mPrerequisites.insert(mpSelector);
          mStatic = mpSelector->isStatic();
        }
      else
        {
          mValid = false;

          if (mpSelector != NULL)
            {
              delete mpSelector;
              mpSelector = NULL;
            }
        }
    }
  else
    {
      mValid = false;
    }
}


// virtual
void CDBFieldSelector::computeProtected()
{
  std::map< CValueList::Type, CValueList > & DBFieldValues = getDBFieldValues();
  DBFieldValues.clear();

  CFieldValueList FieldValueList;
  CFieldValueList ConstraintValueList;

  std::vector< CNode * >::const_iterator itConstraint = mpSelector->beginNodes();
  std::vector< CNode * >::const_iterator endConstraint = mpSelector->endNodes();

  for (; itConstraint != endConstraint; ++itConstraint)
    {
      ConstraintValueList.append(CFieldValue((*itConstraint)->id));
    }

  CQuery::in(mTable, mField, FieldValueList, false, "pid", ConstraintValueList);

  if (FieldValueList.size() > 0)
    {
      DBFieldValues.insert(std::make_pair(FieldValueList.getType(), FieldValueList));
    }

  // std::cout << "CDBFieldSelector::compute: " << FieldValueList.size() << std::endl;
}


