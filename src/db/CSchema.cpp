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

#include "db/CSchema.h"

// static
CSchema CSchema::INSTANCE;

CSchema::CSchema()
  : mId()
  , mLabel()
  , mTables()
  , mValid(false)
{}

CSchema::CSchema(const CSchema & src)
  : mId(src.mId)
  , mLabel(src.mLabel)
  , mTables(src.mTables)
  , mValid(src.mValid)
{}

// virtual
CSchema::~CSchema()
{}

void CSchema::fromJSON(const json_t * json)
{

}

const std::string & CSchema::getId() const
{
  return mId;
}

const bool & CSchema::isValid() const
{
  return mValid;
}

const CTable & CSchema::getTable(const std::string & table) const
{
  static CTable InvalidTable;

  std::map< std::string, CTable >::const_iterator found = mTables.find(table);

  if (found != mTables.end())
    return found->second;

  return InvalidTable;
}

