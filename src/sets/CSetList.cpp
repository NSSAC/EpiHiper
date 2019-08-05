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

#include <iostream>
#include <jansson.h>

#include "CSetList.h"

CSetList::CSetList()
  : std::vector< CSet >()
  , mId2Index()
  , mValid(true)
{}

CSetList::CSetList(const CSetList & src)
  : std::vector< CSet >(src)
  , mId2Index()
  , mValid(src.mValid)

{
  std::vector< CSet >::iterator it = begin();
  std::vector< CSet >::iterator itEnd = end();

  for (size_t i = 0; it != itEnd; ++it, i++)
    {
      mId2Index[it->getId()] = i;
    }
}

// virtual
CSetList::~CSetList()
{}

void CSetList::fromJSON(const json_t * json)
{
  mValid = json_is_array(json);

  if (!mValid) return;

  for (size_t i = 0, imax = json_array_size(json); i < imax; ++i)
    {
      CSet Variable(json_array_get(json, i));
      mValid &= Variable.isValid();
      mId2Index[Variable.getId()] = i;
      push_back(Variable);
    }
}

void CSetList::toBinary(std::ostream & os) const
{
  size_t Size = size();

  os.write(reinterpret_cast<const char *>(&Size), sizeof(size_t));

  std::vector< CSet >::const_iterator it = begin();
  std::vector< CSet >::const_iterator itEnd = end();

  for (; it != itEnd; ++it)
    {
      it->toBinary(os);
    }
}

void CSetList::fromBinary(std::istream & is)
{
  size_t Size;

  is.read(reinterpret_cast<char *>(&Size), sizeof(size_t));

  mValid &= (Size == size());

  if (!mValid) return;

  std::vector< CSet >::iterator it = begin();
  std::vector< CSet >::iterator itEnd = end();

  for (; it != itEnd; ++it)
    {
      it->fromBinary(is);
    }
}

CSet & CSetList::operator[](const size_t & index)
{
  static CSet Invalid(NULL);

  if (index < size())
    return std::vector< CSet >::operator[](index);

  return Invalid;
}

CSet & CSetList::operator[](const std::string & id)
{
  static CSet Invalid(NULL);

  std::map< std::string, size_t >::iterator found = mId2Index.find(id);

  if (found != mId2Index.end())
    return operator[](found->second);

  return Invalid;
}
