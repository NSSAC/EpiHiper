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

#include "actions/CActionEnsemble.h"
#include "sets/CSetContent.h"

CActionEnsemble::CActionEnsemble()
  : mOnce()
  , mForEach()
  , mSampling()
  , mValid(false)
{}

CActionEnsemble::CActionEnsemble(const CActionEnsemble & src)
  : mOnce(src.mOnce)
  , mForEach(src.mForEach)
  , mSampling(src.mSampling)
  , mValid(src.mValid)
{}

CActionEnsemble::CActionEnsemble(const json_t * json)
  : mOnce()
  , mForEach()
  , mSampling()
  , mValid(false)
{
  fromJSON(json);
}

// virtual
CActionEnsemble::~CActionEnsemble()
{}

void CActionEnsemble::fromJSON(const json_t * json)
{
  /*
    "actionEnsemble": {
      "type": "object",
      "minProperties": 1,
      "properties": {
        "once": {
          "description": "A list of actions to be executed once.",
          "type": "array",
          "minItems": 1,
          "items": {"$ref": "#/definitions/action"}
        },
        "foreach": {
          "description": "A list of actions to be executed for each element in the filter.",
          "type": "array",
          "minItems": 1,
          "items": {"$ref": "#/definitions/action"}
        },
        "sampling": {"$ref": "#/definitions/sampling"}
      }
    },
  */

  mValid = true;

  json_t * pValue = json_object_get(json, "once");

  for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i)
    {
      CActionDefinition ActionDefinition(json_array_get(pValue, i));
      mValid &= ActionDefinition.isValid();
      mOnce.push_back(ActionDefinition);
    }

  pValue = json_object_get(json, "foreach");

  for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i)
    {
      CActionDefinition ActionDefinition(json_array_get(pValue, i));
      mValid &= ActionDefinition.isValid();
      mForEach.push_back(ActionDefinition);
    }

  mSampling.fromJSON(json_object_get(json, "sampling"));
  mValid &= mSampling.isValid();
}

const bool & CActionEnsemble::isValid() const
{
  return mValid;
}

void CActionEnsemble::process(const CSetContent & targets) const
{
  std::vector< CActionDefinition >::const_iterator it = mForEach.begin();
  std::vector< CActionDefinition >::const_iterator end = mForEach.end();

  std::set< CEdge * >::const_iterator itEdges = targets.beginEdges();
  std::set< CEdge * >::const_iterator endEdges = targets.endEdges();

  for (; itEdges != endEdges; ++itEdges)
    for (it = mForEach.begin(); it != end; ++it)
      it->process(*itEdges);

  std::set< CNode * >::const_iterator itNodes = targets.beginNodes();
  std::set< CNode * >::const_iterator endNodes = targets.endNodes();

  for (; itNodes != endNodes; ++itNodes)
    for (it = mForEach.begin(); it != end; ++it)
      it->process(*itNodes);

  mSampling.process(targets);
}


