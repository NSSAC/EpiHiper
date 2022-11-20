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

#include <sstream>
#include <limits>
#include <jansson.h>

#include "math/CSizeOf.h"
#include "sets/CSetContent.h"
#include "utilities/CLogger.h"

// static
std::vector< CSizeOf * > CSizeOf::GetInstances()
{
  return INSTANCES;
}

CSizeOf::CSizeOf()
  : CValue(0.0)
  , CComputable()
  , mpSetContent(NULL)
  , mIndex(std::numeric_limits< size_t >::max())
  , mIndentifier()
{}

CSizeOf::CSizeOf(const CSizeOf & src)
  : CValue(src)
  , CComputable(src)
  , mpSetContent(src.mpSetContent != NULL ? src.mpSetContent->copy() : NULL)
  , mIndex(src.mIndex)
  , mIndentifier(src.mIndentifier)
{}

CSizeOf::CSizeOf(const json_t * json)
  : CValue(0.0)
  , CComputable()
  , mpSetContent(NULL)
  , mIndex(std::numeric_limits< size_t >::max())
  , mIndentifier()
{
  fromJSON(json);

  if (mValid)
    {
      mIndex = INSTANCES.size();
      INSTANCES.push_back(this);
    }
}

// virtual
CSizeOf::~CSizeOf()
{
  CSetContent::destroy(mpSetContent);
}

// virtual
CValueInterface * CSizeOf::copy() const
{
  return new CSizeOf(*this);
}

//  virtual
bool CSizeOf::computeProtected()
{
  if (mValid)
    {
      return broadcastSize() == (int) CCommunicate::ErrorCode::Success;
    }

  return false;
}

int CSizeOf::broadcastSize()
{
  // This barrier is necessarry since all threads must have finished computing the set.
#pragma omp barrier
#pragma omp single
  {
    CLogger::setSingle(true);
    size_t LocalSize = mpSetContent->totalSize();
    *static_cast< double * >(mpValue) = LocalSize;

    CCommunicate::ClassMemberReceive< CSizeOf > Receive(this, &CSizeOf::receiveSize);
    CCommunicate::roundRobinFixed(&LocalSize, sizeof(size_t), &Receive);
    CLogger::setSingle(false);
  }

  CLogger::debug() << "CSizeOf: Returned '" << *static_cast< double * >(mpValue) << "' for " << mIndentifier;
  return (int) CCommunicate::ErrorCode::Success;
}

CCommunicate::ErrorCode CSizeOf::receiveSize(std::istream & is, int /* sender */)
{
  size_t RemoteSize;

  is.read(reinterpret_cast< char * >(&RemoteSize), sizeof(size_t));
  *static_cast< double * >(mpValue) += RemoteSize;

  return CCommunicate::ErrorCode::Success;
}

void CSizeOf::fromJSON(const json_t * json)
{
  /*
    "sizeof": {
      "$id": "#sizeof",
      "description": "The count of the elements in a set.",
      "type": "object",
      "required": ["sizeof"],
      "properties": {
        "sizeof": {"$ref": "#/definitions/setContent"}
      }
    },
  */

  mValid = false; // DONE
  mpSetContent = CSetContent::create(json_object_get(json, "sizeof"));

  if (mpSetContent != NULL && mpSetContent->isValid())
    {
      mPrerequisites.insert(mpSetContent);
      mStatic = mpSetContent->isStatic();
      mValid = true;

      char * str = json_dumps(json_object_get(json, "sizeof"), JSON_COMPACT | JSON_INDENT(0));
      mIndentifier = str;
      free(str);

      return;
    }

  if (mpSetContent != NULL)
    {
      delete mpSetContent;
      mpSetContent = NULL;
    }

  CLogger::error("sizeof: Missing or invalid set content.");
}
