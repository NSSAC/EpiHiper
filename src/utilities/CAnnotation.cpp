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
