// BEGIN: Copyright 
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
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
std::string CSetReference::getComputableId() const
{
  if (mpSet != nullptr)
    return mpSet->getComputableId();

  return CSetContent::getComputableId();
}
