// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2024 Rector and Visitors of the University of Virginia 
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

#include <cmath>
#include <cstring>
#include <jansson.h>

#include "actions/CSampling.h"
#include "actions/CActionEnsemble.h"
#include "sets/CSetContent.h"
#include "utilities/CLogger.h"
#include "variables/CVariableList.h"

CSampling::CSampled::CSampled()
  : CSetContent(CSetContent::Type::sampled)
{
  CComputable::Instances.erase(this);
}

CSampling::CSampled::CSampled(const CSampled & src)
  : CSetContent(src)
{
  CComputable::Instances.erase(this);
}

// virtual
CSampling::CSampled::~CSampled()
{}

// virtual 
bool CSampling::CSampled::computeSetContent()
{
  return true;
}


// virtual
void CSampling::CSampled::fromJSONProtected(const json_t * /* json */)
{}

// virtual
bool CSampling::CSampled::lessThanProtected(const CSetContent & rhs) const
{
  return this < static_cast< const CSampling::CSampled * >(&rhs);
}

// virtual 
void CSampling::CSampled::setScopeProtected()
{}

CSampling::CSampling()
  : mType()
  , mpVariable(NULL)
  , mConversionFactor(100.0)
  , mPercentage()
  , mCount()
  , mpSampled(NULL)
  , mpNotSampled(NULL)
  , mpTargets(NULL)
  , mSampledTargets()
  , mNotSampledTargets()
  , mLocalLimit()
  , mpCommunicateBuffer(NULL)
  , mValid(false)
{
  mLocalLimit.init();
}

CSampling::CSampling(const CSampling & src)
  : mType(src.mType)
  , mpVariable(src.mpVariable)
  , mConversionFactor(src.mConversionFactor)
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
  , mpVariable(NULL)
  , mPercentage()
  , mCount()
  , mpSampled(NULL)
  , mpNotSampled(NULL)
  , mpTargets(NULL)
  , mSampledTargets()
  , mNotSampledTargets()
  , mLocalLimit()
  , mpCommunicateBuffer(NULL)
  , mValid(false)
{
  mLocalLimit.init();
  fromJSON(json);
}

// virtual
CSampling::~CSampling()
{
  mLocalLimit.release();

  if (mpSampled != NULL)
    delete mpSampled;

  if (mpNotSampled != NULL)
    delete mpNotSampled;

  if (mpCommunicateBuffer != NULL)
    delete [] mpCommunicateBuffer;
}

// virtual
void CSampling::fromJSON(const json_t * json)
{
  /*
    "sampling": {
      "$id": "#sampling",
      "description": "Sampling within a set with actions for sampled and non-sampled individuals.",
      "allOf": [
        {
          "$ref": "#/definitions/annotation"
        },
        {
          "oneOf": [
            {
              "$id": "#relativeSampling",
              "description": "",
              "type": "object",
              "oneOf": [
                {
                  "required": [
                    "type",
                    "number",
                    "sampled"
                  ]
                },
                {
                  "required": [
                    "type",
                    "variable",
                    "sampled"
                  ]
                },
                {
                  "required": [
                    "type",
                    "number",
                    "nonSampled"
                  ]
                },
                {
                  "required": [
                    "type",
                    "variable",
                    "nonSampled"
                  ]
                }
              ],
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
                "variable": {
                  "type": "object",
                  "required": [
                    "idRef"
                  ],
                  "properties": {
                    "idRef": {
                      "$ref": "#/definitions/uniqueIdRef"
                    }
                  }
                },
                "sampled": {
                  "$ref": "#/definitions/actionEnsemble"
                },
                "nonsampled": {
                  "$ref": "#/definitions/actionEnsemble"
                },
                "patternProperties": {
                  "^ann:": {}
                },
                "additionalProperties": false
              }
            },
            {
              "$id": "#absoluteSampling",
              "description": "",
              "type": "object",
              "oneOf": [
                {
                  "required": [
                    "type",
                    "number",
                    "sampled"
                  ]
                },
                {
                  "required": [
                    "type",
                    "variable",
                    "sampled"
                  ]
                },
                {
                  "required": [
                    "type",
                    "number",
                    "nonSampled"
                  ]
                },
                {
                  "required": [
                    "type",
                    "variable",
                    "nonSampled"
                  ]
                }
              ],
              "properties": {
                "type": {
                  "type": "string",
                  "enum": [
                    "absolute"
                  ]
                },
                "number": {
                  "type": "number",
                  "minimum": 0,
                  "multipleOf": 1.0
                },
                "variable": {
                  "type": "object",
                  "required": [
                    "idRef"
                  ],
                  "properties": {
                    "idRef": {
                      "$ref": "#/definitions/uniqueIdRef"
                    }
                  }
                },
                "sampled": {
                  "$ref": "#/definitions/actionEnsemble"
                },
                "nonsampled": {
                  "$ref": "#/definitions/actionEnsemble"
                },
                "patternProperties": {
                  "^ann:": {}
                },
                "additionalProperties": false
              }
            },
            {
              "$id": "#relativeSamplingProbability",
              "description": "Sampling by given probability",
              "oneOf": [
                {
                  "required": [
                    "type",
                    "probability",
                    "sampled"
                  ]
                },
                {
                  "required": [
                    "type",
                    "probability",
                    "nonsampled"
                  ]
                }
              ],
              "properties": {
                "type": {
                  "type": "string",
                  "enum": [
                    "individual",
                    "group"
                  ]
                },
                "probability": {
                  "type": "object",
                  "oneOf": [
                    {
                      "required": [
                        "number"
                      ],
                      "properties": {
                        "number": {
                          "type": "number",
                          "minimum": 0.0,
                          "maximum": 1.0
                        }
                      }
                    },
                    {
                      "required": [
                        "variable"
                      ],
                      "properties": {
                        "variable": {
                          "type": "object",
                          "required": [
                            "idRef"
                          ],
                          "properties": {
                            "idRef": {
                              "$ref": "#/definitions/uniqueIdRef"
                            }
                          }
                        }
                      },
                      "sampled": {
                        "$ref": "#/definitions/actionEnsemble"
                      },
                      "nonsampled": {
                        "$ref": "#/definitions/actionEnsemble"
                      },
                      "patternProperties": {
                        "^ann:": {}
                      },
                      "additionalProperties": false
                    }
                  ]
                }
              }
            },
            {
              "$id": "#absoluteSamplingProbability",
              "description": "Sampling by given count",
              "oneOf": [
                {
                  "required": [
                    "type",
                    "count",
                    "sampled"
                  ]
                },
                {
                  "required": [
                    "type",
                    "count",
                    "nonsampled"
                  ]
                }
              ],
              "properties": {
                "type": {
                  "type": "string",
                  "enum": [
                    "absolute"
                  ]
                }
              },
              "count": {
                "type": "object",
                "oneOf": [
                  {
                    "required": [
                      "number"
                    ],
                    "properties": {
                      "number": {
                        "type": "number",
                        "minimum": 0.0,
                        "multipleOf": 1
                      }
                    }
                  },
                  {
                    "required": [
                      "variable"
                    ],
                    "properties": {
                      "variable": {
                        "type": "object",
                        "required": [
                          "idRef"
                        ],
                        "properties": {
                          "idRef": {
                            "$ref": "#/definitions/uniqueIdRef"
                          }
                        }
                      },
                      "sampled": {
                        "$ref": "#/definitions/actionEnsemble"
                      },
                      "nonsampled": {
                        "$ref": "#/definitions/actionEnsemble"
                      },
                      "patternProperties": {
                        "^ann:": {}
                      },
                      "additionalProperties": false
                    }
                  }
                ]
              }
            }
          ]
        }
      ]
    },
  */

  mValid = false; // DONE

  if (json == NULL)
    {
      // This is an optional object it is therefor permissable to be undefined
      mValid = true;
      return;
    }

  json_t * pValue = json_object_get(json, "type");

  if (pValue == NULL)
    {
      CLogger::error("Sampling: Missing 'type'.");
      return;
    }

  if (strcmp(json_string_value(pValue), "individual") == 0)
    {
      mType = Type::relativeIndividual;
    }
  else if (strcmp(json_string_value(pValue), "group") == 0)
    {
      mType = Type::relativeGroup;
    }
  else if (strcmp(json_string_value(pValue), "absolute") == 0)
    {
      mType = Type::absolute;
    }
  else
    {
      CLogger::error("Sampling: Invalid 'type'.");
      return;
    }

  const json_t * pSampleValue = NULL;
  mConversionFactor = 1.0;

  switch (mType)
  {
    case Type::relativeGroup:
    case Type::relativeIndividual:
      pSampleValue = json_object_get(json, "probability");

      if (pSampleValue != NULL)
        mConversionFactor = 100.0;
      else
        pSampleValue = json_object_get(json, "percent");

      break;

    case Type::absolute:
      pSampleValue = json_object_get(json, "count");
      break;
  }

  // Fall back to deprecated format
  if (pSampleValue == NULL)
    {
      pSampleValue = json;
      mConversionFactor = 1.0;
    }

  mpVariable = &CVariableList::INSTANCE[pSampleValue];

  if (mpVariable->isValid())
    {
      if (mpVariable->getScope() == CVariable::Scope::global)
        {
          CLogger::error("Sampling: The variable '{}' must have scope local.", mpVariable->getId());
          return;
        }
    }
  else
    {
      mpVariable = NULL;

      pValue = json_object_get(pSampleValue, "number");

      if (!json_is_real(pValue))
        {
          CLogger::error("Sampling: Missing or invalid 'number'.");
          return;
        }

      switch (mType)
        {
        case Type::relativeIndividual:
        case Type::relativeGroup:
          mPercentage = json_real_value(pValue) * mConversionFactor;
          break;

        case Type::absolute:
          mCount = std::round(json_real_value(pValue));
          break;
        }
    }

  pValue = json_object_get(json, "sampled");

  if (pValue != NULL)
    {
      mpSampled = new CActionEnsemble(pValue);

      if (!mpSampled->isValid())
        {
          delete mpSampled;
          mpSampled = NULL;
          CLogger::error("Sampling: Invalid action ensemble for 'sampled'.");

          return;
        }
    }

  pValue = json_object_get(json, "nonsampled");

  if (pValue != NULL)
    {
      mpNotSampled = new CActionEnsemble(pValue);

      if (!mpNotSampled->isValid())
        {
          delete mpNotSampled;
          mpNotSampled = NULL;
          CLogger::error("Sampling: Invalid action ensemble for 'nonsampled'.");

          return;
        }
    }

  mValid = mpSampled != nullptr || mpNotSampled != nullptr;
}

void CSampling::process(const CSetContent & targets)
{
  if (mpSampled == NULL && mpNotSampled == NULL)
    {
      return;
    }

  mpTargets = &targets;
  // For types relativeGroup and absolute we need to communicate with the other compute nodes to determine the local number
  // even if targets are empty;

  if (mType == Type::relativeGroup || mType == Type::absolute)
    {
      mLocalLimit.Active() = mpTargets->size();

      // This barrier is required since we have to have all threads determine the number of samples they require 
#pragma omp barrier
#pragma omp single
      {
        CLogger::setSingle(true);
        broadcastCount();
        CLogger::setSingle(false);
      }

      mpTargets->sampleMax(mLocalLimit.Active(), mSampledTargets, mNotSampledTargets);
    }
  else
    {
      if (mpVariable != NULL)
        mPercentage = mpVariable->toValue().toNumber() * mConversionFactor;

      if (std::isnan(mPercentage))
        {
          CLogger::warn("CSampling: Percentage evaluates to NaN, no items will be sampled."); 
          mPercentage = 0.0;
        }

      mpTargets->samplePercent(mPercentage, mSampledTargets, mNotSampledTargets);
    }

  CLogger::info("CSampling: Sampled set size: '{}', Not sampled set size: '{}'.", mSampledTargets.size(), mNotSampledTargets.size());

  if (mpSampled != NULL)
    mpSampled->process(mSampledTargets);
  else
    CLogger::info("CActionEnsemble: Target set contains '{}' ignored items.", mSampledTargets.size());
  

  if (mpNotSampled != NULL)
    mpNotSampled->process(mNotSampledTargets);
  else
    CLogger::info("CActionEnsemble: Target set contains '{}' ignored items.", mNotSampledTargets.size());
}

int CSampling::broadcastCount()
{
  if (mpCommunicateBuffer == NULL)
    mpCommunicateBuffer = new size_t[CCommunicate::TotalProcesses()];

  mLocalLimit = mpTargets->sizes();
  size_t * pIt = mLocalLimit.beginThread();
  size_t * pEnd = mLocalLimit.endThread();
  size_t * pSize = mpCommunicateBuffer;

  for (; pIt != pEnd; ++pIt, ++pSize)
    *pSize = *pIt;

  if (CCommunicate::MPIProcesses > 1)
    {
      CCommunicate::ClassMemberReceive< CSampling > Receive(this, &CSampling::receiveCount);
      CCommunicate::master(0, mpCommunicateBuffer, CCommunicate::LocalProcesses() * sizeof(size_t), CCommunicate::TotalProcesses() * sizeof(size_t), &Receive);
    }
  else
    {
      determineThreadLimits();
    }

  mLocalLimit.Master() = 0;  

  pIt = mLocalLimit.beginThread();
  pSize = mpCommunicateBuffer + CCommunicate::MPIRank * CCommunicate::LocalProcesses();

  for (; pIt != pEnd; ++pIt, ++pSize)
    {
      *pIt = *pSize;

      if (mLocalLimit.isThread(pIt))
        mLocalLimit.Master() += *pIt;
    }

  return (int) CCommunicate::ErrorCode::Success;
}

CCommunicate::ErrorCode CSampling::receiveCount(std::istream & is, int sender)
{
  if (CCommunicate::MPIRank == 0)
    if (sender != CCommunicate::MPIRank)
      {
        is.read(reinterpret_cast< char * >(mpCommunicateBuffer + sender * CCommunicate::LocalProcesses()), CCommunicate::LocalProcesses() * sizeof(size_t));
      }
    else
      {
        // This will always be last call for the central rank, i.e., we now have all sizes.
        size_t * pIt = mLocalLimit.beginThread();
        size_t * pEnd = mLocalLimit.endThread();
        size_t * pSize = mpCommunicateBuffer + CCommunicate::MPIRank * CCommunicate::LocalProcesses();

        for (; pIt != pEnd; ++pIt, ++pSize)
          *pSize = *pIt;

        determineThreadLimits();
      }
  else
    {
      is.read(reinterpret_cast< char * >(mpCommunicateBuffer), CCommunicate::TotalProcesses() * sizeof(size_t));
    }

  return CCommunicate::ErrorCode::Success;
}

const bool & CSampling::isValid() const
{
  return mValid;
}

bool CSampling::isEmpty() const
{
  return mpSampled == NULL
         && mpNotSampled == NULL;
}

void CSampling::determineThreadLimits()
{
  // We now determine the numbers for all process
  double Requested = 0.0;
  double Available = 0.0;
  double Target = 0.0;
  double Allowed = 0.0;
  double Error = 0.0;

  size_t * pRemote = mpCommunicateBuffer;
  size_t * pRemoteEnd = mpCommunicateBuffer + CCommunicate::TotalProcesses();

  for (; pRemote != pRemoteEnd; ++pRemote)
    {
      Requested += *pRemote;
    }

  if (mType == Type::relativeGroup)
    {
      if (mpVariable != NULL)
        mPercentage = mpVariable->toValue().toNumber() * mConversionFactor;

      Available = std::round(Requested * mPercentage / 100.0);
    }
  else
    {
      if (mpVariable != NULL)
        Available = std::round(mpVariable->toValue().toNumber());
      else  
        Available = mCount;
    }

  if (std::isnan(Available))
    {
      CLogger::warn("CSampling: Available evaluates to NaN, no items will be sampled."); 

      for (pRemote = mpCommunicateBuffer; pRemote != pRemoteEnd; ++pRemote)
        {
          *pRemote = 0;
        }
    }
  else if (Available < Requested)
    {
      // We need to limit the individual counts;
      for (pRemote = mpCommunicateBuffer; pRemote != pRemoteEnd; ++pRemote)
        if (Available > 0.5)
          {
            Target = ((double) *pRemote) * Available / Requested;
            Allowed = round(Target);

            // Accumulate the error
            Error += Allowed - Target;

            // Keep the total error within (-1, 1)
            if (Error >= 1.0)
              {
                Error -= 1.0;
                Allowed -= 1.0;
              }
            else if (Error <= -1.0)
              {
                Error += 1.0;
                Allowed += 1.0;
              }

            Requested -= *pRemote;
            *pRemote = Allowed;
            Available -= *pRemote;

            if (Available < -0.5)
              {
                *pRemote -= 1;
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