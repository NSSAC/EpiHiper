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

CSet::CSet(const CSet & src)
  : CSetContent(src)
  , CAnnotation(src)
  , mId(src.mId)
  , mType(src.mType)
  , mpSetContent(CSetContent::copy(src.mpSetContent))
  , mValid(src.mValid)
{}

CSet::CSet(const json_t * json)
  : CSetContent()
  , CAnnotation()
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

// virtual
void CSet::fromJSON(const json_t * json)
{
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
      mPrerequisites.insert(mpSetContent);
      mValid &= (mpSetContent != NULL && mpSetContent->isValid());
    }
  else
    {
      mValid = false;
    }

  CAnnotation::fromJSON(json);
}

// virtual
void CSet::compute()
{}

// virtual
bool CSet::contains(CNode * pNode) const
{
  if (mValid)
    return mpSetContent->contains(pNode);

  return CSetContent::contains(pNode);
}

// virtual
bool CSet::contains(CEdge * pEdge) const
{
  if (mValid)
    return mpSetContent->contains(pEdge);

  return CSetContent::contains(pEdge);
}

// virtual
bool CSet::contains(const CValueInterface & value) const
{
  if (mValid)
    return mpSetContent->contains(value);

  return CSetContent::contains(value);
}

// virtual
std::set< CEdge * >::const_iterator CSet::beginEdges() const
{
  if (mValid)
    return mpSetContent->beginEdges();

  return CSetContent::beginEdges();
}

// virtual
std::set< CEdge * >::const_iterator CSet::endEdges() const
{
  if (mValid)
    return mpSetContent->endEdges();

  return CSetContent::endEdges();
}

// virtual
std::set< CNode * >::const_iterator CSet::beginNodes() const
{
  if (mValid)
    return mpSetContent->beginNodes();

  return CSetContent::beginNodes();
}

// virtual
std::set< CNode * >::const_iterator CSet::endNodes() const
{
  if (mValid)
    return mpSetContent->endNodes();

  return CSetContent::endNodes();
}

// virtual
const std::set< CEdge * > & CSet::getEdges() const
{
  if (mValid)
    return mpSetContent->getEdges();

  return CSetContent::getEdges();
}

// virtual
const std::set< CNode * > & CSet::getNodes() const
{
  if (mValid)
    return mpSetContent->getNodes();

  return CSetContent::getNodes();
}

const std::map< CValueList::Type, CValueList > & CSet::getDBFieldValues() const
{
  if (mValid)
    return mpSetContent->getDBFieldValues();

  return CSetContent::getDBFieldValues();
}


