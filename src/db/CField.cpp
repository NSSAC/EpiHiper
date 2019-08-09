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

#include "db/CField.h"

CField::CField()
  : mId()
  , mLabel()
  , mType()
  , mValid(false)
{}

CField::CField(const CField &src)
  : mId(src.mId)
  , mLabel(src.mLabel)
  , mType(src.mType)
  , mValid(src.mValid)
{}

// virtual
CField::~CField()
{}

void CField::fromJSON(const json_t * json)
{}

const std::string & CField::getId() const
{
  return mId;
}

const bool & CField::isValid() const
{
  return mValid;
}

const CValue::Type & CField::getType() const
{
  return mType;
}

