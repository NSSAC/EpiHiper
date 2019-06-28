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

#include "Enum.h"

Enum::Enum()
  : Annotation()
  , mId()
  , mMask(0)
  , mValid(false)
{}

Enum::Enum(const Enum & src)
  : Annotation(src)
  , mId(src.mId)
  , mMask(src.mMask)
  , mValid(src.mValid)
{}


Enum::~Enum()
{}

void Enum::fromJSON(const json_t * json)
{
  json_t * pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
      mId = json_string_value(pValue);
      mAnnId = mId;
    }

  mValid = !mId.empty();

  Annotation::fromJSON(json);
}

const std::string & Enum::getId() const
{
  return mId;
}

const bool & Enum::isValid() const
{
  return mValid;
}

void Enum::setMask(const TraitData::base & mask)
{
  mMask = mask;
}

const TraitData::base & Enum::getMask() const
{
  return mMask;
}

