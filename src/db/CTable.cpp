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

 #include "db/CTable.h"

CTable::CTable()
  : mId()
  , mLabel()
  , mFields()
  , mValid(false)
{}

CTable::CTable(const CTable &src)
  : mId(src.mId)
  , mLabel(src.mLabel)
  , mFields(src.mFields)
  , mValid(src.mValid)
{}

// virtual
CTable::~CTable()
{}

void CTable::fromJSON(const json_t * json)
{

}

const std::string & CTable::getId() const
{
  return mId;
}

const bool & CTable::isValid() const
{
  return mValid;
}

const CField & CTable::getField(const std::string & field) const
{
  static CField Invalid;

  std::map< std::string, CField >::const_iterator found = mFields.find(field);

  if (found != mFields.end())
    return found->second;

  return Invalid;
}
