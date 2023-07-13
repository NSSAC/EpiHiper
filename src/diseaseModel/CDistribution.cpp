// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2023 Rector and Visitors of the University of Virginia 
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

#include <jansson.h>

#include "diseaseModel/CDistribution.h"
#include "utilities/CLogger.h"
#include "utilities/CSimConfig.h"

CDistribution::CDistribution()
  : mType(Type::__NONE)
  , mDiscrete()
  , mUniformSet()
  , mpSample(NULL)
  , mFixed(0.0)
  , mUniformInt()
  , mUniformReal()
  , mNormal()
  , mGamma()
  , mValid()
  , mNormalizedJSON("{}")
{}

CDistribution::CDistribution(const CDistribution & src)
  : mType(src.mType)
  , mDiscrete(src.mDiscrete)
  , mUniformSet(src.mUniformSet)
  , mpSample(src.mpSample)
  , mFixed(0.0)
  , mUniformInt(src.mUniformInt)
  , mUniformReal(src.mUniformReal)
  , mNormal(src.mNormal)
  , mGamma(src.mGamma)
  , mValid(src.mValid)
  , mNormalizedJSON(src.mNormalizedJSON)
{}

// virtual
CDistribution::~CDistribution()
{}

void CDistribution::fromJSON(const json_t * json)
{
  /*
    "distribution": {
      "allOf": [
        {"$ref": "#/definitions/annotation"},
        {
          "type": "object",
          "description": "Specify distributions for sampling values.",
          "$comment": "Note: The result may require rounding if necessary.",
          "oneOf": [
            {
              "required": ["fixed"]
            },
            {
              "required": ["discrete"]
            },
            {
              "required": ["uniform"]
            },
            {
              "required": ["normal"]
            },
            {
              "required": ["gamma"]
            }
          ],
          "properties": {
            "fixed": {"$ref": "#/definitions/nonNegativeNumber"},
            "discrete": {
              "description": "A number is sampled from the array with each number having the given probability.",
              "type": "array",
              "items": {
                "type": "object",
                "required": [
                  "probability",
                  "value"
                ],
                "properties": {
                  "probability": {
                    "allOf": [
                      {"$ref": "#/definitions/nonNegativeNumber"},
                      {"maximum": 1}
                    ]
                  },
                  "value": {"$ref": "#/definitions/nonNegativeNumber"}
                }
              }
            },
            "uniform": {
              "oneOf": [
                {
                  "type": "array",
                  "description": "A number is sampled uniformly from the given values",
                  "items": {"$ref": "#/definitions/nonNegativeNumber"}
                },
                {
                  "type": "object",
                  "description": "A number is sampled uniformly in the given interval [min, max]",
                  "required": [
                    "min",
                    "max"
                  ],
                  "properties": {
                    "min": {"$ref": "#/definitions/nonNegativeNumber"},
                    "max": {"$ref": "#/definitions/nonNegativeNumber"}
                  }
                }
              ]
            },
            "normal": {
              "type": "object",
              "description": "A number is sampled from a normal or Gaussian distribution.",
              "required": [
                "mean",
                "standardDeviation"
              ],
              "properties": {
                "mean": {"$ref": "#/definitions/nonNegativeNumber"},
                "standardDeviation": {"$ref": "#/definitions/nonNegativeNumber"}
              }
            },
            "gamma": {
              "type": "object",
              "description": "A number is sampled from a Gamma distribution.",
              "required": [
                "alpha",
                "beta"
              ],
              "properties": {
                "alpha": {
                  "description": "shape parameter",
                  "$ref": "#/definitions/nonNegativeNumber"
                },
                "beta": {
                  "description": "scale parameter",
                  "$ref": "#/definitions/nonNegativeNumber"
                }
              }
            }
          }
        }
      ]
    },
  */

  mValid = false; 
  mNormalizedJSON = "{}";
  mType = Type::__NONE;

  json_t * pValue = json_object_get(json, "fixed");

  if (json_is_real(pValue))
    {
      mType = Type::fixed;
      mpSample = &CDistribution::fixed;
      int Fixed = std::round(json_real_value(pValue)); 
      mFixed = Fixed;

      if (Fixed < 0)
        {
          CLogger::error("Distribution fixed: Invalid value.");
          return;
        }

      mValid = true;
    }

  pValue = json_object_get(json, "discrete");

  if (json_is_array(pValue))
    {
      mType = Type::discrete;
      mpSample = &CDistribution::discrete;

      double Total = 0.0;
      mDiscrete.resize(json_array_size(pValue));
      std::vector< std::pair< double, unsigned int > >::iterator it = mDiscrete.begin();
      std::vector< std::pair< double, unsigned int > >::iterator end = mDiscrete.end();

      for (size_t i = 0; it != end; ++it, ++i)
        {
          json_t * pObject = json_array_get(pValue, i);

          if (json_is_object(pObject))
            {
              json_t * pReal = json_object_get(pObject, "probability");

              if (json_is_real(pReal))
                {
                  it->first = json_real_value(pReal);
                  Total += it->first;

                  if (it->first < 0)
                    {
                      CLogger::error("Distribution discrete: Negative value 'probability' for item '{}'.", i);
                      return;
                    };
                }
              else
                {
                  CLogger::error("Distribution discrete: Invalid or missing 'probability' for item '{}'.", i);
                  return;
                }

              pReal = json_object_get(pObject, "value");

              if (json_is_real(pReal))
                {
                  int value  = std::round(json_real_value(pReal));
                  it->second = value;

                  if (value < 0)
                    {
                      CLogger::error("Distribution discrete: Negative value 'value' for item '{}'.", i);
                      return;
                    };
                  
                }
              else
                {
                  CLogger::error("Distribution discrete: Invalid or missing 'value' for item '{}'.", i);
                  return;
                }
            }
        }

      if (fabs(Total - 1.0) > 100.0 * std::numeric_limits< double >::epsilon())
        {
          CLogger::error("Distribution discrete: The sum of probabilities '{}' is not 1.", Total);
          return;
        }

      mValid = true;
      mUniformReal.init(0, std::nextafter(Total, 2.0));
    }

  pValue = json_object_get(json, "uniform");

  if (json_is_array(pValue))
    {
      mType = Type::uniform;
      mpSample = &CDistribution::uniformSet;

      mUniformSet.resize(json_array_size(pValue));
      std::vector< unsigned int >::iterator it = mUniformSet.begin();
      std::vector< unsigned int >::iterator end = mUniformSet.end();

      for (size_t i = 0; it != end; ++it, ++i)
        {
          json_t * pReal = json_array_get(pValue, i);

          if (json_is_real(pReal))
            {
              int value = std::round(json_real_value(pReal));
              *it = value;

              if (value < 0)
                {
                  CLogger::error("Distribution uniform: Negative value for item '{}'.", i);
                  return;
                };
            }
          else
            {
              CLogger::error("Distribution uniform: Invalid value for item '{}'.", i);
              return;
            }
        }

      mValid = true;
      mUniformInt.init(0, mUniformSet.size() - 1);
    }
  else if (json_is_object(pValue))
    {
      mType = Type::uniform;
      mpSample = &CDistribution::uniformDiscrete;

      int min;
      json_t * pReal = json_object_get(pValue, "min");

      if (json_is_real(pReal))
        {
          min = std::round(json_real_value(pReal));

          if (min < 0)
            {
              CLogger::error("Distribution uniform: Negative value for 'min'.");
              return;
            }
        }
      else
        {
          CLogger::error("Distribution uniform: Invalid or missing 'min'.");
          return;
        }

      int max;
      pReal = json_object_get(pValue, "max");

      if (json_is_real(pReal))
        {
          max = std::round(json_real_value(pReal));

          if (max <= min)
            {
              CLogger::error("Distribution uniform: Invalid interval [{}, {}].", min, max);
              return;
            }
        }
      else
        {
          CLogger::error("Distribution uniform: Invalid or missing 'max'.");
          return;
        }

      mValid = true;
      mUniformInt.init(min, max);
    }

  pValue = json_object_get(json, "normal");

  if (json_is_object(pValue))
    {
      mType = Type::normal;
      mpSample = &CDistribution::normal;

      double mean;
      json_t * pReal = json_object_get(pValue, "mean");

      if (json_is_real(pReal))
        {
          mean = json_real_value(pReal);

          if (mean < 0)
            {
              CLogger::error("Distribution normal: Negative value for 'mean'.");
              return;
            }
        }
      else
        {
          CLogger::error("Distribution normal: Invalid or missing 'mean'.");
          return;
        }

      double standardDeviation;
      pReal = json_object_get(pValue, "standardDeviation");

      if (json_is_real(pReal))
        {
          standardDeviation = json_real_value(pReal);
          
          if (standardDeviation < 0)
            {
              CLogger::error("Distribution normal: Negative value for 'standardDeviation'.");
              return;
            }
        }
      else
        {
          CLogger::error("Distribution normal: Invalid or missing 'standardDeviation'.");
          return;
        }

      mValid = true;
      mNormal.init(mean, standardDeviation);
    }

  pValue = json_object_get(json, "gamma");

  if (json_is_object(pValue))
    {
      mType = Type::gamma;
      mpSample = &CDistribution::gamma;

      double alpha;
      json_t * pReal = json_object_get(pValue, "alpha");

      if (json_is_real(pReal))
        {
          alpha = json_real_value(pReal);

          if (alpha < 0)
            {
              CLogger::error("Distribution gamma: Negative value for 'alpha'.");
              return;
            }
        }
      else
        {
          CLogger::error("Distribution gamma: Invalid or missing 'gamma'.");
          return;
        }

      double beta;
      pReal = json_object_get(pValue, "beta");

      if (json_is_real(pReal))
        {
          beta = json_real_value(pReal);
          
          if (beta < 0)
            {
              CLogger::error("Distribution gamma: Negative value for 'beta'.");
              return;
            }
        }
      else
        {
          CLogger::error("Distribution gamma: Invalid or missing 'beta'.");
          return;
        }

      mValid = true;
      mGamma.init(alpha, beta);
    }

  normalizeJSON();
}

const bool & CDistribution::isValid() const
{
  return mValid;
}

unsigned int CDistribution::sample() const
{
  return (this->*mpSample)();
}

unsigned int CDistribution::fixed() const
{
  return mFixed;
}

unsigned int CDistribution::discrete() const
{
  double A = mUniformReal.sample();

  std::vector< std::pair< double, unsigned int > >::const_iterator it = mDiscrete.begin();
  std::vector< std::pair< double, unsigned int > >::const_iterator end = mDiscrete.end();

  for (; it != end; ++it)
    {
      A -= it->first;

      if (A < 0.0)
        return it->second;
    }

  return mDiscrete.rbegin()->second;
}

unsigned int CDistribution::uniformSet() const
{
  return mUniformSet[mUniformInt.sample()];
}

unsigned int CDistribution::uniformDiscrete() const
{
  return mUniformInt.sample();
}

unsigned int CDistribution::normal() const
{
  return std::round(std::max(0.0, mNormal.sample()));
}

unsigned int CDistribution::gamma() const
{
  return std::round(std::max(0.0, mGamma.sample()));
}

void CDistribution::normalizeJSON()
{
  if (mType == Type::__NONE)
    mNormalizedJSON = "{}";

  json_error_t error;
  json_t * pJson = json_loads("{}", JSON_DECODE_INT_AS_REAL, &error);
  json_t * pDistribution = nullptr;

  switch (mType)
    {
    case Type::fixed:
      json_object_set_new(pJson, "fixed", json_real(mFixed));
      break;

    case Type::discrete:
      pDistribution = json_array();

      for (const std::pair< double, unsigned int > & item: mDiscrete) 
        {
          json_t * pItem = json_object();
          json_object_set_new(pItem, "probability", json_real(item.first));
          json_object_set_new(pItem, "value", json_real(item.second));
          json_array_append_new(pDistribution, pItem);
        }

      json_object_set_new(pJson, "discrete", pDistribution);
      break;

    case Type::uniform:
      if (mpSample == &CDistribution::uniformSet)
        {
          pDistribution = json_array();

          for (const unsigned int & item : mUniformSet)
            json_array_append_new(pDistribution, json_real(item));

          json_object_set_new(pJson, "uniform", pDistribution);
        }
      else
        {
          pDistribution = json_object();
          
          json_object_set_new(pDistribution, "min", json_real(mUniformInt.Active().distribution.min()));
          json_object_set_new(pDistribution, "max", json_real(mUniformInt.Active().distribution.max()));
          
          json_object_set_new(pJson, "uniform", pDistribution);
        }
      break;

    case Type::normal:
      pDistribution = json_object();

      json_object_set_new(pDistribution, "mean", json_real(mNormal.Active().distribution.mean()));
      json_object_set_new(pDistribution, "standardDeviation", json_real(mNormal.Active().distribution.stddev()));

      json_object_set_new(pJson, "normal", pDistribution);
      break;

    case Type::gamma:
      pDistribution = json_object();

      json_object_set_new(pDistribution, "alpha", json_real(mGamma.Active().distribution.alpha()));
      json_object_set_new(pDistribution, "beta", json_real(mGamma.Active().distribution.beta()));

      json_object_set_new(pJson, "gamma", pDistribution);
      break;

    case Type::__NONE:
      break;
    }

  mNormalizedJSON = CSimConfig::jsonToString(pJson);
  json_decref(pJson);
}

std::string & CDistribution::getJson()
{
  return mNormalizedJSON;
}

bool CDistribution::setJson(const std::string & json)
{
  if (json == mNormalizedJSON)
    return false;

  json_error_t error;
  json_t * pJson = json_loads(json.c_str(), JSON_DECODE_INT_AS_REAL, &error);
  fromJSON(pJson);
  json_decref(pJson);

  return true;
}
