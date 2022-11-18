// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
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

#include <cstring>
#include <jansson.h>

#include "math/CObservable.h"
#include "diseaseModel/CModel.h"
#include "diseaseModel/CHealthState.h"
#include "actions/CActionQueue.h"
#include "network/CNetwork.h"
#include "utilities/CLogger.h"

bool CObservable::ObservableKey::operator < (const CObservable::ObservableKey & rhs) const
{
  if (id != rhs.id)
    return id < rhs.id;

  if (type != rhs.type)
    return type < rhs.type;

  return subset < rhs.subset;
}

// static
CObservable * CObservable::get(const CObservable::ObservableKey & key)
{
  ObservableMap::iterator found = Observables.find(key);

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

  ObservableKey Key = {pNew->mId, pNew->mObservableType, pNew->mObservableSubset};
  CObservable * pExisting = get(Key);

  if (pExisting == NULL)
    {
      Observables[Key] = pNew;
      return pNew;
    }

  delete pNew;
  return pExisting;
}

CObservable::CObservable()
  : CValue(false)
  , CComputable()
  , mObservableType(ObservableType::healthStateAbsolute)
  , mObservableSubset(ObservableSubset::current)
  , mId(-1)
  , mpCompute(NULL)
{
  mValid = false; // DONE
}

CObservable::CObservable(const CObservable & src)
  : CValue(src)
  , CComputable(src)
  , mObservableType(src.mObservableType)
  , mObservableSubset(src.mObservableSubset)
  , mId(src.mId)
  , mpCompute(src.mpCompute)
{}

CObservable::CObservable(const json_t * json)
  : CValue(false)
  , CComputable()
  , mObservableType(ObservableType::healthStateAbsolute)
  , mObservableSubset(ObservableSubset::current)
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
    {
      bool success = true;

#pragma omp single      
      success = (this->*mpCompute)();

      return success;
    }

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
  double TotalPopulation = CNetwork::Context.Active().getGlobalNodeCount();
  assignValue(&TotalPopulation);

  return true;
}

bool CObservable::computeHealthStateAbsolute()
{
  const CHealthState * pHealthState = CModel::StateFromType(mId);

  if (pHealthState != NULL)
    {
      double Absolute = std::numeric_limits< double >::quiet_NaN();

      switch (mObservableSubset)
        {
        case ObservableSubset::current:
          Absolute = (double) pHealthState->getGlobalCounts().Current;
          break;

        case ObservableSubset::in:
          Absolute = (double) pHealthState->getGlobalCounts().In;
          break;

        case ObservableSubset::out:
          Absolute = (double) pHealthState->getGlobalCounts().Out;
          break;
        }

      assignValue(&Absolute);

      return true;
    }
  
  return false;
}

bool CObservable::computeHealthStateRelative()
{
  const CHealthState * pHealthState = CModel::StateFromType(mId);

  if (pHealthState != NULL)
    {
      double Relative = std::numeric_limits< double >::quiet_NaN();

      switch (mObservableSubset)
        {
        case ObservableSubset::current:
          Relative = (double) pHealthState->getGlobalCounts().Current / CNetwork::Context.Active().getGlobalNodeCount();
          break;

        case ObservableSubset::in:
          Relative = (double) pHealthState->getGlobalCounts().In / CNetwork::Context.Active().getGlobalNodeCount();
          break;

        case ObservableSubset::out:
          Relative = (double) pHealthState->getGlobalCounts().Out / CNetwork::Context.Active().getGlobalNodeCount();
          break;
        }

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
                },
                "subset": {
                  "type": "string",
                  "enum": [
                    "in",
                    "out",
                    "current"
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
          mType = Type::number;
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

      pValue = json_object_get(pObservable, "subset");

      if (!json_is_string(pValue)
          || strcmp(json_string_value(pValue), "current") == 0)
        {
          mObservableSubset = ObservableSubset::current;
        }
      else if (strcmp(json_string_value(pValue), "in") == 0)
        {
          mObservableSubset = ObservableSubset::in;
        }
      else if (strcmp(json_string_value(pValue), "out") == 0)
        {
          mObservableSubset = ObservableSubset::out;
        }
      else
        {
          CLogger::error() << "Observable: Invalid subset '" << json_string_value(pValue) << "'.";
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

      mId = pHealthState->getIndex();
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
