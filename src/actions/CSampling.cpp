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

#include <cmath>
#include <cstring>
#include <jansson.h>

#include "actions/CSampling.h"
#include "actions/CActionEnsemble.h"
#include "sets/CSetContent.h"

CSampling::CSampling()
  : mType()
  , mPercentage()
  , mCount()
  , mpSampled(NULL)
  , mpNotSampled(NULL)
  , mpTargets(NULL)
  , mSampledTargets()
  , mNotSampledTargets()
  , mLocalLimit(0)
  , mpCommunicateBuffer(NULL)
  , mValid(false)
{}

CSampling::CSampling(const CSampling & src)
  : mType(src.mType)
  , mPercentage(src.mPercentage)
  , mCount(src.mCount)
  , mpSampled(src.mpSampled != NULL ? new CActionEnsemble(*src.mpSampled) : NULL)
  , mpNotSampled(src.mpNotSampled != NULL ? new CActionEnsemble(*src.mpNotSampled) : NULL)
  , mpTargets(src.mpTargets)
  , mSampledTargets(src.mSampledTargets)
  , mNotSampledTargets(src.mNotSampledTargets)
  , mLocalLimit(src.mLocalLimit)
  , mpCommunicateBuffer(NULL)
  , mValid(src.mValid)
{}

CSampling::CSampling(const json_t * json)
  : mType()
  , mPercentage()
  , mCount()
  , mpSampled(NULL)
  , mpNotSampled(NULL)
  , mpTargets(NULL)
  , mSampledTargets()
  , mNotSampledTargets()
  , mLocalLimit(0)
  , mpCommunicateBuffer(NULL)
  , mValid(false)
{
  fromJSON(json);
}

// virtual
CSampling::~CSampling()
{}

// virtual
void CSampling::fromJSON(const json_t *json)
{
  /*
    "sampling": {
      "$id": "#sampling",
      "description": "Sampling within a set with actions for sampled and non-sampled individuals.",
      "allOf": [
        {"$ref": "#/definitions/annotation"},
        {
          "oneOf": [
            {
              "$id": "#relativeSampling",
              "description": "",
              "type": "object",
              "required": [
                "type",
                "number"
              ],
              "minProperties": 3,
              "additionalProperties": false,
              "properties": {
                "type": {
                  "type": "string",
                  "enum": [
                    "individual",
                    "group"
                  ]
                },
                "number": {
                  "type": "number",
                  "minimum": 0,
                  "maximum": 100
                },
                "sampled": {"$ref": "#/definitions/actionEnsemble"},
                "nonsampled": {"$ref": "#/definitions/actionEnsemble"}
              }
            },
            {
              "$id": "#absoluteSampling",
              "description": "",
              "type": "object",
              "required": [
                "type",
                "number"
              ],
              "minProperties": 3,
              "additionalProperties": false,
              "properties": {
                "type": {
                  "type": "string",
                  "enum": ["absolute"]
                },
                "number": {
                  "type": "number",
                  "minimum": 0,
                  "multipleOf": 1.0
                },
                "sampled": {"$ref": "#/definitions/actionEnsemble"},
                "nonsampled": {"$ref": "#/definitions/actionEnsemble"}
              }
            }
          ]
        }
      ]
    },
   */

  mValid = true;

  if (json == NULL)
    {
      return;
    }

  json_t * pValue = json_object_get(json, "type");

  if (pValue == NULL)
    {
      mValid = false;
      return;
    }

  if (strcmp(json_string_value(pValue), "individual") == 0)
    {
      mType = Type::relativeIndividual;
    }
  else if(strcmp(json_string_value(pValue), "group") == 0)
    {
      mType = Type::relativeGroup;
    }
  else if(strcmp(json_string_value(pValue), "absolute") == 0)
    {
      mType = Type::absolute;
    }
  else
    {
      mValid = false;
      return;
    }

  pValue = json_object_get(json, "number");

  if (!json_is_real(pValue))
    {
      mValid = false;
      return;
    }

  switch (mType)
  {
    case Type::relativeIndividual:
    case Type::relativeGroup:
      mPercentage = json_real_value(pValue);
      break;

    case Type::absolute:
      mCount = std::round(json_real_value(pValue));
      break;
  }

  pValue = json_object_get(json, "sampled");

  if (pValue != NULL)
    {
      mpSampled = new CActionEnsemble(pValue);
      mValid = mpSampled->isValid();

      if (!mValid)
        {
          delete mpSampled;
          mpSampled = NULL;

          return;
        }
    }

  pValue = json_object_get(json, "notsampled");

  if (pValue != NULL)
    {
      mpNotSampled = new CActionEnsemble(pValue);
      mValid = mpNotSampled->isValid();

      if (!mValid)
        {
          delete mpNotSampled;
          mpNotSampled = NULL;

          return;
        }
    }
}

void CSampling::process(const CSetContent & targets)
{
  if (mpSampled == NULL &&
      mpNotSampled == NULL)
    {
      return;
    }

  mpTargets = &targets;
  // For types relativeGroup and absolute we need to communcate with the other comput nodes to determine the local number
  // even if targets are empty;

  if (mType == Type::relativeGroup ||
      mType == Type::absolute)
    {
      broadcastCount();
      mpTargets->sampleMax(mLocalLimit, mSampledTargets, mNotSampledTargets);
    }
  else
    {
      mpTargets->samplePercent(mPercentage, mSampledTargets, mNotSampledTargets);
    }

  if (mpSampled != NULL)
    {
      mpSampled->process(mSampledTargets);
    }

  if (mpNotSampled != NULL)
    {
      mpNotSampled->process(mNotSampledTargets);
    }
}

int CSampling::broadcastCount()
{
  if (mpCommunicateBuffer == NULL)
    mpCommunicateBuffer = new size_t[CCommunicate::MPIProcesses];

  *mpCommunicateBuffer = mpTargets->size();

  CCommunicate::ClassMemberReceive< CSampling > Receive(this, &CSampling::receiveCount);
  CCommunicate::central(0, mpCommunicateBuffer, sizeof(size_t), CCommunicate::MPIProcesses * sizeof(size_t), &Receive);

  mLocalLimit = *(mpCommunicateBuffer + CCommunicate::MPIRank);

  return (int) CCommunicate::ErrorCode::Success;
}

CCommunicate::ErrorCode CSampling::receiveCount(std::istream & is, int sender)
{
  if (CCommunicate::MPIRank == 0)
    if (sender != CCommunicate::MPIRank)
      {
        is.read(reinterpret_cast<char *>(mpCommunicateBuffer + sender), sizeof(size_t));
      }
    else
      {
        // This will always be last call for the central rank, i.e., we now have all sizes.
        mpCommunicateBuffer[CCommunicate::MPIRank] = mpTargets->size();

        // We now determine the numbers for all process
        double Requested = 0.0;
        double Available = mCount;
        double Allowed = 0.0;

        size_t * pRemote = mpCommunicateBuffer;
        size_t * pRemoteEnd = mpCommunicateBuffer + CCommunicate::MPIProcesses;

        for (; pRemote != pRemoteEnd; ++pRemote)
          {
            Requested += *pRemote;
          }

        if (mType == Type::relativeGroup)
          {
            Available = std::round(Requested * mPercentage / 100.0);
          }

        if (Available < Requested)
          {
            // We need to limit the individual counts;
            for (pRemote = mpCommunicateBuffer; pRemote != pRemoteEnd; ++pRemote)
              if (Available > 0.5)
                {
                  Allowed = std::round(((double) *pRemote) * Available / Requested);
                  Requested -= *pRemote;
                  *pRemote = Allowed;
                  Available -= *pRemote;

                  if (Available < -0.5)
                    {
                      *pRemote += 1;
                      Requested += 1;
                      Available += 1;
                    }
                }
              else
                {
                  *pRemote = 0;
                }
          }
      }
  else
    {
      is.read(reinterpret_cast<char *>(mpCommunicateBuffer), CCommunicate::MPIProcesses * sizeof(size_t));
    }

  return CCommunicate::ErrorCode::Success;
}

const bool & CSampling::isValid() const
{
  return mValid;
}
