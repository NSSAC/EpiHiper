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

#include "CSet.h"

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

CSet::CSet(const CSet & src)
  : CAnnotation(src)
  , mId()
  , mType()
  , mValid(true)
{}

CSet::CSet(const json_t * json)
  : CAnnotation()
  , mId()
  , mType()
  , mValid(true)
{

  CAnnotation::fromJSON(json);
}

// virtual
CSet::~CSet()
{}

void CSet::fromJSON(const json_t * json)
{

}

void CSet::toBinary(std::ostream & os) const
{

}

void CSet::fromBinary(std::istream & is)
{

}

const std::string & CSet::getId() const
{
  return mId;
}

const bool & CSet::isValid() const
{
  return mValid;
}
