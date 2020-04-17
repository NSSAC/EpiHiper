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

#include <cstring>
#include <jansson.h>

#include "math/CObservable.h"
#include "diseaseModel/CModel.h"
#include "diseaseModel/CHealthState.h"
#include "actions/CActionQueue.h"
#include "network/CNetwork.h"
#include "utilities/CLogger.h"

// static
CObservable * CObservable::get(const CObservable::ObservableType & observableType, const size_t & id)
{
  ObservableMap::iterator found = Observables.find(std::make_pair(id, observableType));

  if (found != Observables.end())
    return found->second;

  return NULL;
}

// static
CObservable * CObservable::get(const json_t * json)
{
  CObservable * pNew = new CObservable(json);

  if (!pNew->isValid())
    {
      delete pNew;
      return NULL;
    }

  CObservable * pExisting = get(pNew->mObservableType, pNew->mId);

  if (pExisting == NULL)
    {
      Observables[std::make_pair(pNew->mId, pNew->mObservableType)] = pNew;
      return pNew;
    }

  delete pNew;
  return pExisting;
}

CObservable::CObservable()
  : CValue(false)
  , CComputable()
  , mObservableType()
  , mId(-1)
  , mpCompute(NULL)
{
  mValid = false; // DONE
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

  if (mValid)
    {
      mPrerequisites.insert(&CActionQueue::getCurrentTick());
    }
}

// virtual
CObservable::~CObservable()
{}

// virtual
bool CObservable::computeProtected()
{
  if (mValid
      && mpCompute != NULL)
    return (this->*mpCompute)();

  return false;
}

bool CObservable::operator<(const CObservable & rhs) const
{
  if (mObservableType != rhs.mObservableType)
    return mObservableType < rhs.mObservableType;

  return mId < rhs.mId;
}

bool CObservable::computeTime()
{
  double Time = CActionQueue::getCurrentTick();
  assignValue(&Time);

  return true;
}

bool CObservable::computeTotalPopulation()
{
  double TotalPopulation = CNetwork::INSTANCE->getGlobalNodeCount();
  assignValue(&TotalPopulation);

  return true;
}

bool CObservable::computeHealthStateAbsolute()
{
  const CHealthState * pHealthState = CModel::StateFromType(mId);

  if (pHealthState != NULL)
    {
      assignValue(&pHealthState->getGlobalCounts().Current);

      return true;
    }
  
  return false;
}

bool CObservable::computeHealthStateRelative()
{
  const CHealthState * pHealthState = CModel::StateFromType(mId);

  if (pHealthState != NULL)
    {
      double Relative = ((double) pHealthState->getGlobalCounts().Current) / CNetwork::INSTANCE->getGlobalNodeCount();
      assignValue(&Relative);

      return true;
    }

  return false;
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
            },
            {
              "description": "The total population is number of nodes as specified in contact network JSON header.",
              "type": "string",
              "enum": ["totalPopulation"]
            }
          ]
        }
      }
    },
  */

  mValid = false; // DONE
  json_t * pObservable = json_object_get(json, "observable");

  if (json_is_object(pObservable))
    {
      json_t * pValue = json_object_get(pObservable, "type");

      if (!json_is_string(pValue))
        {
          CLogger::error("Observable: Invalid or missing 'type'.");
          return;
        }

      if (strcmp(json_string_value(pValue), "absolute") == 0)
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
          CLogger::error() << "Observable: Invalid type '" << json_string_value(pValue) << "'.";
          return;
        }

      pValue = json_object_get(pObservable, "healthState");

      if (!json_is_string(pValue))
        {
          CLogger::error("Observable: Invalid or missing 'healthState'.");
          return;
        }

      const CHealthState * pHealthState = CModel::GetState(json_string_value(pValue));

      if (pHealthState == NULL)
        {
          CLogger::error() << "Observable: Invalid healthState '" << json_string_value(pValue) << "'.";
          return;
        }

      mId = CModel::StateToType(pHealthState);
      mValid = true;
      return;
    }
  else if (json_is_string(pObservable))
    {
      if (strcmp(json_string_value(pObservable), "time") == 0)
        {
          mObservableType = ObservableType::time;
          mpCompute = &CObservable::computeTime;
          mId = 0;

          destroyValue();
          mType = Type::number;
          mpValue = createValue(mType);

          mValid = true;
          return;
        }
      else if (strcmp(json_string_value(pObservable), "totalPopulation") == 0)
        {
          mObservableType = ObservableType::totalPopulation;
          mpCompute = &CObservable::computeTotalPopulation;
          mId = 0;
          mStatic = true;

          destroyValue();
          mType = Type::number;
          mpValue = createValue(mType);

          mValid = true;
          return;
        }
      else
        {
          CLogger::error() << "Observable: Invalid observable '" << json_string_value(pObservable) << "'.";
          return;
        }
    }

  return;
}
