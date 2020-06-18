// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

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
{}

CSizeOf::CSizeOf(const CSizeOf & src)
  : CValue(src)
  , CComputable(src)
  , mpSetContent(src.mpSetContent != NULL ? src.mpSetContent->copy() : NULL)
  , mIndex(src.mIndex)
{}

CSizeOf::CSizeOf(const json_t * json)
  : CValue(0.0)
  , CComputable()
  , mpSetContent(NULL)
  , mIndex(std::numeric_limits< size_t >::max())
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
  size_t LocalSize = mpSetContent->size();
  *static_cast< double * >(mpValue) = LocalSize;

  CCommunicate::ClassMemberReceive< CSizeOf > Receive(this, &CSizeOf::receiveSize);
  CCommunicate::roundRobinFixed(&LocalSize, sizeof(size_t), &Receive);

  CLogger::debug() << "CSizeOf: Returned '" << *static_cast< double * >(mpValue) << "'.";
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
      return;
    }

  if (mpSetContent != NULL)
    {
      delete mpSetContent;
      mpSetContent = NULL;
    }

  CLogger::error("sizeof: Missing or invalid set content.");
}
