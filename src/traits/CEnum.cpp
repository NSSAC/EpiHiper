// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
//  
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal 
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions: 
//  
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software. 
//  
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE 
// END: Copyright 

#include <jansson.h>

#include "traits/CEnum.h"

CEnum::CEnum(const std::string & id)
  : CAnnotation()
  , mId(id)
  , mMask(0)
  , mValid(!id.empty())
{
  mAnnId = mId;
}

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

