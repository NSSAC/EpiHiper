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

#include <limits>
#include <jansson.h>

#include "math/CSizeOf.h"
#include "sets/CSetContent.h"

// static
std::vector< CSizeOf * > CSizeOf::INSTANCES;

// static
std::vector< CSizeOf * > CSizeOf::GetInstances()
{
  return INSTANCES;
}

CSizeOf::CSizeOf()
  : CValue(std::numeric_limits< double >::quiet_NaN())
  , CComputable()
  , mpSetContent(NULL)
  , mIndex(std::numeric_limits< size_t >::max())
  , mValid(false)
{}

CSizeOf::CSizeOf(const CSizeOf & src)
  : CValue(src)
  , CComputable(src)
  , mpSetContent(CSetContent::copy(src.mpSetContent))
  , mIndex(src.mIndex)
  , mValid(src.mValid)
{}

CSizeOf::CSizeOf(const json_t * json)
  : CValue(std::numeric_limits< double >::quiet_NaN())
  , CComputable()
  , mpSetContent(NULL)
  , mIndex(std::numeric_limits< size_t >::max())
  , mValid(false)
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
void CSizeOf::compute()
{
  if (mValid)
    {
      *static_cast< double * >(mpValue) = mpSetContent->size();
    }
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

  mpSetContent = CSetContent::create(json_object_get(json, "sizeof"));
  mValid = (mpSetContent != NULL && mpSetContent->isValid());

  if (mValid)
    mPrerequisites.insert(mpSetContent);
}
