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

#include "sets/CSetContent.h"
#include "sets/CEdgeElementSelector.h"
#include "sets/CNodeElementSelector.h"
#include "sets/CDBFieldSelector.h"
#include "sets/CSetOperation.h"
#include "sets/CSetList.h"

CSetContent::CSetContent()
  : mNodes()
  , mEdges()
  , mDBFieldValues()
  , mValid(true)
{}

CSetContent::CSetContent(const CSetContent & src)
  : mNodes(src.mNodes)
  , mEdges(src.mEdges)
  , mDBFieldValues(src.mDBFieldValues)
  , mValid(src.mValid)
{}

CSetContent::~CSetContent()
{}

// static
CSetContent * CSetContent::create(const json_t * json)
{
  json_t * pContentType = json_object_get(json, "elementType");

  if (json_is_string(pContentType))
    {
      if (strcmp(json_string_value(pContentType), "edge") == 0)
        {
          return new CEdgeElementSelector(json);
        }
      else if (strcmp(json_string_value(pContentType), "node") == 0)
        {
          return new CNodeElementSelector(json);
        }
      else if (strcmp(json_string_value(pContentType), "dbField") == 0)
        {
          return new CDBFieldSelector(json);
        }
      else
        {
          return NULL;
        }
    }

  pContentType = json_object_get(json, "operation");

  if (json_is_string(pContentType))
    {
      return new CSetOperation(json);
    }

  pContentType = json_object_get(json, "set");

  if (json_is_string(pContentType))
    {
      json_t * pIdRef = json_object_get(json, "idRef");

      if (json_is_string(pIdRef))
        {
          return CSetList::INSTANCE[json_string_value(pIdRef)].getSetContent();
        }

      return NULL;
    }

  return NULL;
}

// static
CSetContent * CSetContent::copy(const CSetContent * pSetContent)
{
  if (dynamic_cast< const CEdgeElementSelector * >(pSetContent) != NULL)
    return new CEdgeElementSelector(*static_cast< const CEdgeElementSelector * >(pSetContent));

  if (dynamic_cast< const CNodeElementSelector * >(pSetContent) != NULL)
    return new CNodeElementSelector(*static_cast< const CNodeElementSelector * >(pSetContent));

  if (dynamic_cast< const CDBFieldSelector * >(pSetContent) != NULL)
    return new CDBFieldSelector(*static_cast< const CDBFieldSelector * >(pSetContent));

  if (dynamic_cast< const CSetOperation * >(pSetContent) != NULL)
    return new CSetOperation(*static_cast<const  CSetOperation * >(pSetContent));

  return NULL;
}

// static
void CSetContent::destroy(CSetContent *& pSetContent)
{
  if (pSetContent != NULL &&
      dynamic_cast< CSet * >(pSetContent) == NULL)
    {
      delete pSetContent;
      pSetContent = NULL;
    }
}

const bool & CSetContent::isValid() const
{
  return mValid;
}

// virtual
void CSetContent::fromJSON(const json_t * json)
{
  mValid = false;
}

bool CSetContent::contains(CNode * pNode) const
{
  return (mNodes.find(pNode) != mNodes.end());
}

bool CSetContent::contains(CEdge * pEdge) const
{
  return (mEdges.find(pEdge) != mEdges.end());
}

bool CSetContent::contains(const CValueInterface & value) const
{
  std::map< CValueList::Type, CValueList >::const_iterator found = mDBFieldValues.find(value.getType());

  if (found != mDBFieldValues.end())
    return found->second.contains(value);

  return false;
}

