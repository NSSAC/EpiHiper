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

#include "utilities/CAnnotation.h"

CAnnotation::CAnnotation()
  : mAnnId()
  , mAnnLabel()
  , mAnnDescription()
{}

CAnnotation::CAnnotation(const CAnnotation & src)
  : mAnnId(src.mAnnId)
  , mAnnLabel(src.mAnnLabel)
  , mAnnDescription(src.mAnnDescription)
{}

// virtual
CAnnotation::~CAnnotation()
{}

CAnnotation & CAnnotation::operator =(const CAnnotation & rhs)
{
  if (this != &rhs)
    {
      mAnnId = rhs.mAnnId;
      mAnnLabel = rhs.mAnnLabel;
      mAnnDescription = rhs.mAnnDescription;
    }

  return *this;
}

void CAnnotation::fromJSON(const json_t * json)
{
  /*
    "ann:id": {"$ref": "#/definitions/uniqueIdRef"},
    "ann:label": {"$ref": "#/definitions/label"},
    "ann:description": {
  */

  json_t * pValue = json_object_get(json, "ann:id");

  if (json_is_string(pValue))
    {
      mAnnId = json_string_value(pValue);
    }

  pValue = json_object_get(json, "ann:label");

  if (json_is_string(pValue))
    {
      mAnnLabel = json_string_value(pValue);
    }

  pValue = json_object_get(json, "ann:description");

  if (json_is_string(pValue))
    {
      mAnnDescription = json_string_value(pValue);
    }
}

const std::string & CAnnotation::getAnnId() const
{
  return mAnnId;
}

const std::string & CAnnotation::getAnnLabel() const
{
  return mAnnLabel;
}

const std::string & CAnnotation::getAnnDescription() const
{
  return mAnnDescription;
}
