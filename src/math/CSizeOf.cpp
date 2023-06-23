// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2023 Rector and Visitors of the University of Virginia 
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
#include "utilities/CSimConfig.h"
 
bool  CSizeOf::Compare::operator()(const CSizeOf::shared_pointer & lhs, const CSizeOf::shared_pointer & rhs) const
{
  return CSetContent::Compare()(lhs->mpSetContent, rhs->mpSetContent);
}

// static 
std::set< CSizeOf::shared_pointer, CSizeOf::Compare > CSizeOf::Unique;

// static 
CSizeOf::shared_pointer CSizeOf::FromJSON(const json_t * json )
{
  CSizeOf * pNew = new CSizeOf(json);

  if (!pNew->isValid())
    {
      delete pNew;
      return nullptr;
    }

  shared_pointer New(pNew);

  return *Unique.insert(New).first;
}

CSizeOf::CSizeOf()
  : CValue(0.0)
  , CComputable()
  , mpSetContent(NULL)
  , mIndex(std::numeric_limits< size_t >::max())
  , mIdentifier()
{}

CSizeOf::CSizeOf(const CSizeOf & src)
  : CValue(src)
  , CComputable(src)
  , mpSetContent(src.mpSetContent)
  , mIndex(src.mIndex)
  , mIdentifier(src.mIdentifier)
{}

CSizeOf::CSizeOf(const json_t * json)
  : CValue(0.0)
  , CComputable()
  , mpSetContent(NULL)
  , mIndex(std::numeric_limits< size_t >::max())
  , mIdentifier()
{
  fromJSON(json);
}

// virtual
CSizeOf::~CSizeOf()
{}

//  virtual
bool CSizeOf::computeProtected()
{
  if (isValid())
    {
      return broadcastSize() == (int) CCommunicate::ErrorCode::Success;
    }

  return false;
}

int CSizeOf::broadcastSize()
{
  // This barrier is necessary since all threads must have finished computing the set.
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

  CLogger::debug("CSizeOf: Returned '{}' for {}", *static_cast< double * >(mpValue), mIdentifier);
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

  CComputable::mValid = false; // DONE
  mpSetContent = CSetContent::create(json_object_get(json, "sizeof"));

  if (mpSetContent != NULL && mpSetContent->isValid())
    {
      mPrerequisites.insert(mpSetContent.get()); // DONE
      CComputable::mValid = true;

      mIdentifier = CSimConfig::jsonToString(json_object_get(json, "sizeof"));

      return;
    }

  mpSetContent.reset();

  CLogger::error("sizeof: Missing or invalid set content.");
}

// virtual 
bool CSizeOf::isValid() const
{
  return CValue::mValid && CComputable::mValid;
}

