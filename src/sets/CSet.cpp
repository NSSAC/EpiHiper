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

#include <jansson.h>

#include "sets/CSet.h"
#include "network/CNetwork.h"
#include "network/CNode.h"
#include "network/CEdge.h"
#include "sets/CSetContent.h"

/*
        {"$ref": "#/definitions/annotation"},
        {
          "type": "object",
          "required": [
            "id",
            "content",
            "scope"
          ],
          "properties": {
            "id": {"$ref": "#/definitions/uniqueId"},
            "content": {"$ref": "#/definitions/setContent"},
            "scope": {"$ref": "#/definitions/scope"}
          },
          "patternProperties": {
            "^ann:": {}
          },
          "additionalProperties": false
        }
 */

CSet::CSet(const CSet & src)
  : CAnnotation(src)
  , mId(src.mId)
  , mType(src.mType)
  , mpSetContent(CSetContent::copy(src.mpSetContent))
  , mValid(src.mValid)
{}

CSet::CSet(const json_t * json)
  : CAnnotation()
  , mId()
  , mType()
  , mpSetContent(NULL)
  , mValid(true)
{
  fromJSON(json);
}

// virtual
CSet::~CSet()
{
  CSetContent::destroy(mpSetContent);
}

const std::string & CSet::getId() const
{
  return mId;
}

const bool & CSet::isValid() const
{
  return mValid;
}

CSetContent * CSet::getSetContent() const
{
  return mpSetContent;
}


// virtual
void CSet::fromJSON(const json_t * json)
{
  json_t * pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
      mId = json_string_value(pValue);
      mAnnId = mId;
    }

  mValid &= !mId.empty();

  pValue = json_object_get(json, "scope");

  if (json_is_string(pValue))
    {
      std::string(json_string_value(pValue));
      mType = std::string(json_string_value(pValue)) == "global" ? Type::global : Type::local;
    }
  else
    {
      mValid = false;
    }

  pValue = json_object_get(json, "content");

  if (json_is_object(pValue))
    {
      mpSetContent = CSetContent::create(pValue);
      mValid &= (mpSetContent != NULL && mpSetContent->isValid());
    }
  else
    {
      mValid = false;
    }

  CAnnotation::fromJSON(json);
}


