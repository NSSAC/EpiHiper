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

#include <jansson.h>

#include "sets/CSetReference.h"
#include "sets/CSetList.h"
#include "math/CDependencyGraph.h"
#include "utilities/CLogger.h"

CSetReference::CSetReference()
  : CSetContent()
  , mIdRef()
  , mpSet(NULL)
{}

CSetReference::CSetReference(const CSetReference & src)
  : CSetContent(src)
  , mIdRef(src.mIdRef)
  , mpSet(src.mpSet)
{}

CSetReference::CSetReference(const json_t * json)
  : CSetContent()
  , mIdRef()
  , mpSet(NULL)
{
  fromJSON(json);
}

// virtual
CSetReference::~CSetReference()
{}

// virtual
CSetContent * CSetReference::copy() const
{
  return new CSetReference(*this);
}

// virtual
void CSetReference::fromJSON(const json_t * json)
{
  /*
    "setReference": {
      "$id": "#setReference",
      "description": "A reference to a set.",
      "type": "object",
      "required": ["set"],
      "properties": {
        "set": {
          "type": "object",
          "required": ["idRef"],
          "properties": {
            "idRef": {"$ref": "#/definitions/uniqueIdRef"}
          }
        }
      }
  */

  mValid = false; // DONE
  json_t * pSet = json_object_get(json, "set");

  if (!json_is_object(pSet))
    {
      CLogger::error("Set reference: Missing or invalid value for 'set'.");
      return;
    }

  json_t * pIdRef = json_object_get(pSet, "idRef");

  if (!json_is_string(pIdRef))
    {
      CLogger::error("Set reference: Missing or invalid value for 'idRef'.");
      return;
    }

  mIdRef = json_string_value(pIdRef);
  mpSet = NULL;
  mValid = true;
  UnResolved.push_back(this);
}

// static
bool CSetReference::resolve()
{
  bool success = true;
  std::vector< CSetReference * >::iterator it = UnResolved.begin();
  std::vector< CSetReference * >::iterator end = UnResolved.end();

  for (; it != end; ++it)
    {
      (*it)->mpSet = CSetList::INSTANCE[(*it)->mIdRef];

      if ((*it)->mpSet != NULL
          && (*it)->mpSet->isValid())
        {
          (*it)->mPrerequisites.insert((*it)->mpSet);
          (*it)->mStatic = (*it)->mpSet->isStatic();
        }
      else
        {
          (*it)->mpSet = NULL;
          (*it)->mValid = false; // DONE
          CLogger::error() << "Set reference: Unresolved idRef '" << (*it)->mIdRef << "'.";
          success = false;
        }
    }

  return success;
}

// virtual
bool CSetReference::computeProtected()
{
  CLogger::debug("CSetReference: No operation.");
  return true;
}

// virtual
bool CSetReference::contains(CNode * pNode) const
{
  if (mValid)
    return mpSet->contains(pNode);

  return CSetContent::contains(pNode);
}

// virtual
bool CSetReference::contains(CEdge * pEdge) const
{
  if (mValid)
    return mpSet->contains(pEdge);

  return CSetContent::contains(pEdge);
}

// virtual
bool CSetReference::contains(const CValueInterface & value) const
{
  if (mValid)
    return mpSet->contains(value);

  return CSetContent::contains(value);
}

// virtual
std::vector< CEdge * >::const_iterator CSetReference::beginEdges() const
{
  if (mValid)
    return mpSet->beginEdges();

  return CSetContent::beginEdges();
}

// virtual
std::vector< CEdge * >::const_iterator CSetReference::endEdges() const
{
  if (mValid)
    return mpSet->endEdges();

  return CSetContent::endEdges();
}

// virtual
std::vector< CNode * >::const_iterator CSetReference::beginNodes() const
{
  if (mValid)
    return mpSet->beginNodes();

  return CSetContent::beginNodes();
}

// virtual
std::vector< CNode * >::const_iterator CSetReference::endNodes() const

{
  if (mValid)
    return mpSet->endNodes();

  return CSetContent::endNodes();
}

// virtual
const std::vector< CEdge * > & CSetReference::getEdges() const
{
  if (mValid)
    return mpSet->getEdges();

  return CSetContent::getEdges();
}

// virtual
const std::vector< CNode * > & CSetReference::getNodes() const
{
  if (mValid)
    return mpSet->getNodes();

  return CSetContent::getNodes();
}

const std::map< CValueList::Type, CValueList > & CSetReference::getDBFieldValues() const
{
  if (mValid)
    return mpSet->getDBFieldValues();

  return CSetContent::getDBFieldValues();
}

// virtual
std::vector< CEdge * > & CSetReference::getEdges()
{
  if (mValid)
    return mpSet->getEdges();

  return CSetContent::getEdges();
}

// virtual
std::vector< CNode * > & CSetReference::getNodes()
{
  if (mValid)
    return mpSet->getNodes();

  return CSetContent::getNodes();
}

std::map< CValueList::Type, CValueList > & CSetReference::getDBFieldValues()
{
  if (mValid)
    return mpSet->getDBFieldValues();

  return CSetContent::getDBFieldValues();
}
