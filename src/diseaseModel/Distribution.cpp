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

#include <string>
#include <jansson.h>

#include "Distribution.h"

Distribution::Distribution()
  : mType(Type::__NONE)
  , mDiscrete()
  , mUniformSet()
  , mValid(false)
  , mpSample(NULL)
  , mFixed(0.0)
  , mpUniformInt(NULL)
  , mpUniformReal(NULL)
  , mpNormal(NULL)
{}

Distribution::Distribution(const Distribution & src)
  : mType(src.mType)
  , mDiscrete(src.mDiscrete)
  , mUniformSet(src.mUniformSet)
  , mValid(src.mValid)
  , mpSample(src.mpSample)
  , mFixed(0.0)
  , mpUniformInt(src.mpUniformInt != NULL ? new Random::uniform_int(*src.mpUniformInt) : NULL)
  , mpUniformReal(src.mpUniformReal != NULL ? new Random::uniform_real(*src.mpUniformReal) : NULL)
  , mpNormal(src.mpNormal != NULL ? new Random::normal(*src.mpNormal) : NULL)
{}

// virtual
Distribution::~Distribution()
{
  if (mpUniformInt != NULL) delete mpUniformInt;
  if (mpUniformReal != NULL) delete mpUniformReal;
  if (mpNormal != NULL) delete mpNormal;
}

void Distribution::fromJSON(const json_t * json)
{
  mValid = true;

  json_t * pValue = json_object_get(json, "fixed");

  if (json_is_real(pValue))
    {
       mType = Type::fixed;
       mpSample = &Distribution::fixed;
       mFixed = std::round(json_real_value(pValue));

       mValid &= (mFixed >= 0);
    }

  pValue = json_object_get(json, "discrete");

  if (json_is_array(pValue))
    {
       mType = Type::discrete;
       mpSample = &Distribution::discrete;

       double Total = 0.0;
       mDiscrete.resize(json_array_size(pValue));
       std::vector< std::pair < double, unsigned int > >::iterator it = mDiscrete.begin();
       std::vector< std::pair < double, unsigned int > >::iterator end = mDiscrete.end();

       for (size_t i = 0; it != end; ++it, ++i)
         {
           json_t * pObject = json_array_get(pValue, i);

           if (json_is_object(pObject))
             {
               json_t * pReal = json_object_get(pObject, "probability");

               if (json_is_real(pReal))
                 {
                   it->first = json_real_value(pReal);
                   mValid &= (it->first >= 0);
                   Total += it->first;
                 }
               else
                 {
                   mValid = false;
                 }

              pReal = json_object_get(pObject, "value");

              if (json_is_real(pReal))
                {
                  it->second = std::round(json_real_value(pReal));
                  mValid &= (it->first >= 0);
                }
              else
                {
                  mValid = false;
                }
             }
         }

       mValid &= fabs(Total - 1.0) < 100.0 * std::numeric_limits< double >::epsilon();

       if (mValid)
         {
           mpUniformReal = new Random::uniform_real(0, std::nextafter(Total, 2.0));
         }
    }

  pValue = json_object_get(json, "uniform");

  if (json_is_array(pValue))
    {
      mType = Type::uniform;
      mpSample = &Distribution::uniformSet;

      mUniformSet.resize(json_array_size(pValue));
      std::vector< unsigned int >::iterator it = mUniformSet.begin();
      std::vector< unsigned int >::iterator end = mUniformSet.end();

      for (size_t i = 0; it != end; ++it, ++i)
        {
          json_t * pReal = json_array_get(pValue, i);

          if (json_is_real(pReal))
            {
              *it = std::round(json_real_value(pReal));
              mValid &= (*it >= 0);
            }
          else
            {
              mValid = false;
            }
        }

      if (mValid)
        {
          mpUniformInt = new Random::uniform_int(0, mUniformSet.size() - 1);
        }
    }
  else if (json_is_object(pValue))
    {
      mType = Type::uniform;
      mpSample = &Distribution::uniformDiscrete;

      unsigned int min;
      json_t * pReal = json_object_get(pValue, "min");

      if (json_is_real(pReal))
        {
          min = std::round(json_real_value(pReal));
          mValid &= (min >= 0);
        }
      else
        {
          mValid = false;
        }

      unsigned int max;
      pReal = json_object_get(pValue, "max");

      if (json_is_real(pReal))
        {
          max = std::round(json_real_value(pReal));
          mValid &= (max >= 0);
        }
      else
        {
          mValid = false;
        }

      if (mValid)
        {
          mpUniformInt = new Random::uniform_int(min, max);
        }
     }

  pValue = json_object_get(json, "normal");

  if (json_is_object(pValue))
    {
      mType = Type::normal;
      mpSample = &Distribution::normal;

      double mean;
      json_t * pReal = json_object_get(pValue, "mean");

      if (json_is_real(pReal))
        {
          mean = json_real_value(pReal);
          mValid &= (mean >= 0);
        }
      else
        {
          mValid = false;
        }

      double standardDeviation;
      pReal = json_object_get(pValue, "standardDeviation");

      if (json_is_real(pReal))
        {
          standardDeviation = json_real_value(pReal);
          mValid &= (standardDeviation >= 0);
        }
      else
        {
          mValid = false;
        }

      if (mValid)
        {
          mpNormal = new Random::normal(mean, standardDeviation);
        }
    }

  mValid &= (mType != Type::__NONE);

}

const bool & Distribution::isValid() const
{
  return mValid;
}

unsigned int Distribution::sample() const
{
  return (this->*mpSample)();
}

unsigned int Distribution::fixed() const
{
  return mFixed;
}

unsigned int Distribution::discrete() const
{
  double A = mpUniformReal->operator()(Random::G);

  std::vector< std::pair < double, unsigned int > >::const_iterator it = mDiscrete.begin();
  std::vector< std::pair < double, unsigned int > >::const_iterator end = mDiscrete.end();

  for (; it != end; ++it)
    {
      A -= it->first;

      if (A < 0.0)
        return it->second;
    }

  return mDiscrete.rbegin()->second;
}

unsigned int Distribution::uniformSet() const
{
  return mUniformSet[mpUniformInt->operator()(Random::G)];
}

unsigned int Distribution::uniformDiscrete() const
{
  return mpUniformInt->operator()(Random::G);
}

unsigned int Distribution::normal() const
{
  return std::round(std::max(0.0, mpNormal->operator()(Random::G)));
}

