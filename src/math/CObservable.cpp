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

#include <cstring>
#include <jansson.h>

#include "math/CObservable.h"
#include "diseaseModel/CModel.h"
#include "diseaseModel/CHealthState.h"
#include "actions/CActionQueue.h"
#include "network/CNetwork.h"

// static
std::set< CObservable > CObservable::Observables;

// static
CObservable * CObservable::get(const CObservable::ObservableType & observableType, const size_t & id)
{
  CObservable Observable(observableType, id);
  std::set< CObservable >::iterator found = Observables.find(Observable);

  if (found != Observables.end())
    return const_cast< CObservable * >(&(*found));

  return NULL;
}

// static
CObservable * CObservable::get(const json_t * json)
{
  CObservable Observable(json);

  return const_cast< CObservable * >(&(*Observables.insert(Observable).first));
}

CObservable::CObservable()
  : CValue(false)
  , CComputable()
  , mObservableType()
  , mId(-1)
  , mpCompute(NULL)
{
  mValid = false;
}

CObservable::CObservable(const CObservable & src)
  : CValue(src)
  , CComputable(src)
  , mObservableType(src.mObservableType)
  , mId(src.mId)
  , mpCompute(src.mpCompute)
{}

CObservable::CObservable(const ObservableType & observableType, const size_t & id)
  : CValue(false)
  , CComputable()
  , mObservableType(observableType)
  , mId(id)
  , mpCompute(NULL)
{}

CObservable::CObservable(const json_t * json)
  : CValue(false)
  , CComputable()
  , mObservableType()
  , mId(-1)
  , mpCompute(NULL)
{
  fromJSON(json);
}

// virtual
CObservable::~CObservable()
{}

// virtual
void CObservable::compute()
{
  if (mValid &&
      mpCompute != NULL)
    (this->*mpCompute)();
}

bool CObservable::operator < (const CObservable & rhs) const
{
  if (mObservableType != rhs.mObservableType)
    return mObservableType < rhs.mObservableType;

  return mId < rhs.mId;
}

void CObservable::computeTime()
{
  assignValue(&CActionQueue::getCurrentTick());
}

void CObservable::computeHealthStateAbsolute()
{
  const CHealthState * pHealthState = CModel::stateFromType(mId);

  if (pHealthState != NULL)
    {
      assignValue(&pHealthState->getGlobalCounts().Current);
    }
}

void CObservable::computeHealthStateRelative()
{
  const CHealthState * pHealthState = CModel::stateFromType(mId);

  if (pHealthState != NULL)
    {
      double Relative = ((double) pHealthState->getGlobalCounts().Current)/CNetwork::INSTANCE->getTotalNodes();
      assignValue(&Relative);
    }
}


void CObservable::fromJSON(const json_t * json)
{
  /*
    "observable": {
      "$id": "#observable",
      "description": "An observable which is always accessible within EpiHiper (time and health state statistics) of the current state of the system.",
      "type": "object",
      "required": ["observable"],
      "properties": {
        "observable": {
          "oneOf": [
            {
              "type": "object",
              "required": [
                "healthState",
                "type"
              ],
              "properties": {
                "healthState": {"$ref": "#/definitions/uniqueIdRef"},
                "type": {
                  "type": "string",
                  "enum": [
                    "absolute",
                    "relative"
                  ]
                }
              }
            },
            {
              "description": "The time property references the current simulation time.",
              "type": "string",
              "enum": ["time"]
            }
          ]
        }
      }
    },
  */

  json_t * pObservable = json_object_get(json, "observable");

  if (!json_is_object(pObservable))
    {
      mValid = false;
      return;
    }

  json_t * pValue = json_object_get(pObservable, "type");

  if (!json_is_string(pValue))
    {
      mValid = false;
      return;
    }

  if (strcmp(json_string_value(pValue), "time") == 0)
    {
      mObservableType = ObservableType::time;
      mpCompute = &CObservable::computeTime;
      mId = 0;

      destroyValue();
      mType = Type::id;
      mpValue = createValue(mType);

      mValid = true;
      return;
    }
  else if (strcmp(json_string_value(pValue), "absolute") == 0)
    {
      mObservableType = ObservableType::healthStateAbsolute;
      mpCompute = &CObservable::computeHealthStateAbsolute;

      destroyValue();
      mType = Type::id;
      mpValue = createValue(mType);
    }
  else if (strcmp(json_string_value(pValue), "relative") == 0)
    {
      mObservableType = ObservableType::healthStateRelative;
      mpCompute = &CObservable::computeHealthStateRelative;

      destroyValue();
      mType = Type::number;
      mpValue = createValue(mType);
    }
  else
    {
      mValid = false;
      return;
    }

  pValue = json_object_get(pObservable, "healthState");

  if (!json_is_string(pValue))
    {
      mValid = false;
      return;
    }

  const CHealthState * pHealthState = CModel::getState(json_string_value(pValue));

  if (pHealthState == NULL)
    {
      mValid = false;
      return;
    }

  mId = CModel::stateToType(pHealthState);
  mValid = true;
}

