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

#include "variables/CVariableList.h"

#include <iostream>
#include <jansson.h>


// static
CVariableList CVariableList::INSTANCE;

CVariableList::CVariableList()
  : std::vector< CVariable >()
  , mId2Index()
  , mValid(true)
{}

CVariableList::CVariableList(const CVariableList & src)
  : std::vector< CVariable >(src)
  , mId2Index(src.mId2Index)
  , mValid(src.mValid)
{}

// virtual
CVariableList::~CVariableList()
{}

void CVariableList::fromJSON(const json_t * json)
{
  mValid = json_is_array(json);

  if (!mValid) return;

  for (size_t i = 0, imax = json_array_size(json); i < imax; ++i)
    {
      CVariable Variable(json_array_get(json, i));
      mValid &= Variable.isValid();
      mId2Index[Variable.getId()] = i;
      push_back(Variable);
    }
}

void CVariableList::toBinary(std::ostream & os) const
{
  size_t Size = size();

  os.write(reinterpret_cast<const char *>(&Size), sizeof(size_t));

  std::vector< CVariable >::const_iterator it = begin();
  std::vector< CVariable >::const_iterator itEnd = end();

  for (; it != itEnd; ++it)
    {
      it->toBinary(os);
    }
}

void CVariableList::fromBinary(std::istream & is)
{
  size_t Size;

  is.read(reinterpret_cast<char *>(&Size), sizeof(size_t));

  mValid &= (Size == size());

  if (!mValid) return;

  std::vector< CVariable >::iterator it = begin();
  std::vector< CVariable >::iterator itEnd = end();

  for (; it != itEnd; ++it)
    {
      it->fromBinary(is);
    }
}

CVariable & CVariableList::operator[](const size_t & index)
{
  static CVariable Invalid(NULL);

  if (index < size())
    return std::vector< CVariable >::operator[](index);

  return Invalid;
}

CVariable & CVariableList::operator[](const std::string & id)
{
  static CVariable Invalid(NULL);

  std::map< std::string, size_t >::iterator found = mId2Index.find(id);

  if (found != mId2Index.end())
    return operator[](found->second);

  return Invalid;
}

CVariable & CVariableList::operator[](const json_t * json)
{
  /*
    "variableReference": {
      "$id": "#variableReference",
      "description": "A variable reference.",
      "type": "object",
      "required": ["variable"],
      "properties": {
        "variable": {
          "type": "object",
          "required": ["idRef"],
          "properties": {
            "idRef": {"$ref": "#/definitions/uniqueIdRef"}
          }
        }
      }
    },
  */

  static CVariable Invalid(NULL);
  json_t * pVariable = json_object_get(json, "variable");

  if (!json_is_object(pVariable))
    return Invalid;

  json_t * pIdRef = json_object_get(pVariable, "idRef");

  if (!json_is_string(pIdRef))
    return Invalid;

  return operator[](json_string_value(pIdRef));
}
