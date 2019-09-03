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

CSizeOf::CSizeOf()
  : CValue(std::numeric_limits< double >::quiet_NaN())
  , CComputable()
  , mpSetContent(NULL)
  , mValid(false)
{}

CSizeOf::CSizeOf(const CSizeOf & src)
  : CValue(src)
  , CComputable(src)
  , mpSetContent(CSetContent::copy(src.mpSetContent))
  , mValid(src.mValid)
{}

CSizeOf::CSizeOf(const json_t * json)
  : CValue(std::numeric_limits< double >::quiet_NaN())
  , CComputable()
  , mpSetContent(NULL)
  , mValid(false)
{
  fromJSON(json);
}

// virtual
CSizeOf::~CSizeOf()
{
  CSetContent::destroy(mpSetContent);
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
}
