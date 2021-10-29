// BEGIN: Copyright 
// Copyright (C) 2019 - 2021 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include <string>
#include <jansson.h>

#include "diseaseModel/CDistribution.h"
#include "utilities/CLogger.h"

CDistribution::CDistribution()
  : mType(Type::__NONE)
  , mDiscrete()
  , mUniformSet()
  , mpSample(NULL)
  , mFixed(0.0)
  , mpUniformInt(NULL)
  , mpUniformReal(NULL)
  , mpNormal(NULL)
  , mpGamma(NULL)
  , mValid(false)
{}

CDistribution::CDistribution(const CDistribution & src)
  : mType(src.mType)
  , mDiscrete(src.mDiscrete)
  , mUniformSet(src.mUniformSet)
  , mpSample(src.mpSample)
  , mFixed(0.0)
  , mpUniformInt(src.mpUniformInt != NULL ? new CRandom::uniform_int(*src.mpUniformInt) : NULL)
  , mpUniformReal(src.mpUniformReal != NULL ? new CRandom::uniform_real(*src.mpUniformReal) : NULL)
  , mpNormal(src.mpNormal != NULL ? new CRandom::normal(*src.mpNormal) : NULL)
  , mpGamma(src.mpGamma != NULL ? new CRandom::gamma(*src.mpGamma) : NULL)
  , mValid(src.mValid)
{}

// virtual
CDistribution::~CDistribution()
{
  if (mpUniformInt != NULL)
    delete mpUniformInt;
  if (mpUniformReal != NULL)
    delete mpUniformReal;
  if (mpNormal != NULL)
    delete mpNormal;
  if (mpGamma != NULL)
    delete mpGamma;
}

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
              "description": "A number is sampled from the array with each number having the given propability.",
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
                  "description": "A number is sampled uniformly in the given intervall [min, max]",
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

  mValid = false; // DONE

  json_t * pValue = json_object_get(json, "fixed");

  if (json_is_real(pValue))
    {
      mType = Type::fixed;
      mpSample = &CDistribution::fixed;
      int Fixed = std::round(json_real_value(pValue)); 
      mValid = (Fixed >= 0);
      mFixed = Fixed;

      if (!mValid)
        {
          CLogger::error("Distribution fixed: Invalid value.");
        }
      
      return;
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
                      CLogger::error() << "Distribution discrete: Negative value 'probability' for item '" << i << "'.";
                      return;
                    };
                }
              else
                {
                  CLogger::error() << "Distribution discrete: Invalid or missing 'probability' for item '" << i << "'.";
                  return;
                }

              pReal = json_object_get(pObject, "value");

              if (json_is_real(pReal))
                {
                  int value  = std::round(json_real_value(pReal));
                  it->second = value;

                  if (value < 0)
                    {
                      CLogger::error() << "Distribution discrete: Negative value 'value' for item '" << i << "'.";
                      return;
                    };
                  
                }
              else
                {
                  CLogger::error() << "Distribution discrete: Invalid or missing 'value' for item '" << i << "'.";
                  return;
                }
            }
        }

      mValid = fabs(Total - 1.0) < 100.0 * std::numeric_limits< double >::epsilon();

      if (mValid)
        {
          mpUniformReal = new CRandom::uniform_real(0, std::nextafter(Total, 2.0));
        }
      else
        {
          CLogger::error() << "Distribution discrete: The sum of probabilities '" << Total << "' is not 1.";
        }

      return;
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
                  CLogger::error() << "Distribution uniform: Negative value for item '" << i << "'.";
                  return;
                };
            }
          else
            {
              CLogger::error() << "Distribution uniform: Invalid value for item '" << i << "'.";
              return;
            }
        }

      mValid = true;
      mpUniformInt = new CRandom::uniform_int(0, mUniformSet.size() - 1);
      return;
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
              CLogger::error() << "Distribution uniform: Invalid interval [" << min << ", " << max << "].";
              return;
            }
        }
      else
        {
          CLogger::error("Distribution uniform: Invalid or missing 'max'.");
          return;
        }

      mValid = true;
      mpUniformInt = new CRandom::uniform_int(min, max);
      return;
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
      mpNormal = new CRandom::normal(mean, standardDeviation);
      return;
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
      mpGamma = new CRandom::gamma(alpha, beta);
      return;
    }
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
  double A = mpUniformReal->operator()(CRandom::G.Active());

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
  return mUniformSet[mpUniformInt->operator()(CRandom::G.Active())];
}

unsigned int CDistribution::uniformDiscrete() const
{
  return mpUniformInt->operator()(CRandom::G.Active());
}

unsigned int CDistribution::normal() const
{
  return std::round(std::max(0.0, mpNormal->operator()(CRandom::G.Active())));
}

unsigned int CDistribution::gamma() const
{
  return std::round(std::max(0.0, mpGamma->operator()(CRandom::G.Active())));
}
