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

#include "sets/CSet.h"
#include "network/CNetwork.h"
#include "network/CNode.h"
#include "network/CEdge.h"
#include "utilities/CLogger.h"

// static 
CSet * CSet::empty()
{
  CSet * pEmpty = new CSet();
  pEmpty->mId = "%empty%";
  pEmpty->mType = Type::local;
  pEmpty->mValid = true;
  pEmpty->mStatic = true;

  return pEmpty;
}
  
CSet::CSet()
  : CSetContent()
  , CAnnotation()
  , mId()
  , mType()
  , mpSetContent(NULL)
  , mValid(true)
{}

CSet::CSet(const CSet & src)
  : CSetContent(src)
  , CAnnotation(src)
  , mId(src.mId)
  , mType(src.mType)
  , mpSetContent(src.mpSetContent != NULL ? src.mpSetContent->copy() : NULL)
  , mValid(src.mValid)
{}

CSet::CSet(const json_t * json)
  : CSetContent()
  , CAnnotation()
  , mId()
  , mType()
  , mpSetContent(NULL)
  , mValid(true)
{
  fromJSON(json);
}

// virtual
CSet::~CSet()
{
  CSetContent::destroy(mpSetContent);
}

// virtual
CSetContent * CSet::copy() const
{
  return new CSet(*this);
}

const std::string & CSet::getId() const
{
  return mId;
}

const bool & CSet::isValid() const
{
  return mValid;
}

// virtual
void CSet::fromJSON(const json_t * json)
{
  /*
    {"$ref": "#/definitions/annotation"},
    {
      "type": "object",
      "required": [
        "id",
        "content",
        "scope"
      ],
      "properties": {
        "id": {"$ref": "#/definitions/uniqueId"},
        "content": {"$ref": "#/definitions/setContent"},
        "scope": {"$ref": "#/definitions/scope"}
      },
      "patternProperties": {
        "^ann:": {}
      },
      "additionalProperties": false
    }
  */

  mValid = false; // DONE
  json_t * pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
      mId = json_string_value(pValue);
      mAnnId = mId;
    }

  if (mId.empty())
    {
      CLogger::error("Set: Invalid or missing value for 'id'.");
      return;
    }

  CAnnotation::fromJSON(json);

  pValue = json_object_get(json, "scope");

  if (json_is_string(pValue))
    {
      mType = std::string(json_string_value(pValue)) == "global" ? Type::global : Type::local;
    }
  else
    {
      CLogger::error("Set: Invalid or missing value for 'scope'.");
      return;
    }

  pValue = json_object_get(json, "content");

  if (json_is_object(pValue))
    {
      mpSetContent = CSetContent::create(pValue);

      if (mpSetContent != NULL
          && mpSetContent->isValid())
        {
          mPrerequisites.insert(mpSetContent);
          mStatic = mpSetContent->isStatic();
          mValid = true;
          return;
        }

      if (mpSetContent != NULL)
        {
          delete mpSetContent;
          mpSetContent = NULL;
        }
    }

  CLogger::error("Set: Invalid or missing value for 'content'.");
}

// virtual
bool CSet::computeProtected()
{
  CLogger::debug("CSet: No operation.");
  return true;
}

// virtual 
std::string CSet::getComputableId() const
{
  return "CSet: " + mId;
}