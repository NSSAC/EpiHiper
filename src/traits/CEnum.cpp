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

#include "traits/CEnum.h"

CEnum::CEnum()
  : CAnnotation()
  , mId()
  , mMask(0)
  , mValid(false)
{}

CEnum::CEnum(const CEnum & src)
  : CAnnotation(src)
  , mId(src.mId)
  , mMask(src.mMask)
  , mValid(src.mValid)
{}


CEnum::~CEnum()
{}

void CEnum::fromJSON(const json_t * json)
{
  json_t * pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
      mId = json_string_value(pValue);
      mAnnId = mId;
    }

  mValid = !mId.empty();

  CAnnotation::fromJSON(json);
}

const std::string & CEnum::getId() const
{
  return mId;
}

const bool & CEnum::isValid() const
{
  return mValid;
}

void CEnum::setMask(const CTraitData::base & mask)
{
  mMask = mask;
}

const CTraitData::base & CEnum::getMask() const
{
  return mMask;
}

