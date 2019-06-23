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

/*
 * Traits.cpp
 *
 *  Created on: Jun 21, 2019
 *      Author: shoops
 */

#include <vector>
#include <fstream>
#include <iostream>
#include <cmath>

#include <jansson.h>

#include "Trait.h"

#include "SimConfig.h"

// static
std::map< std::string, Trait > Trait::load(const std::string & jsonFile)
{
  std::map< std::string, Trait > Traits;

  json_t * pRoot = SimConfig::loadJson(jsonFile);

  if (pRoot == NULL)
    {
      return Traits;
    }

  // Get the key "traits"
  json_t * pTraits = json_object_get(pRoot, "traits");

  // Iterate of the array elements
  for (size_t i = 0, imax = json_array_size(pTraits); i < imax; ++i)
    {
      Trait Trait;
      Trait.fromJSON(json_array_get(pTraits, i));

      if (Trait.isValid())
        {
          Traits[Trait.getId()] = Trait;
        }
    }

  json_decref(pRoot);

  return Traits;
}

Trait::Trait()
  : Annotation()
  , mId()
  , mBytes(0)
  , mFeatureMap()
  , mValid(false)
{}

Trait::~Trait() {}

Trait::Trait(const Trait & src)
  : Annotation(src)
  , mId(src.mId)
  , mBytes(src.mBytes)
  , mFeatureMap(src.mFeatureMap)
  , mValid(src.mValid)
{}

void Trait::fromJSON(const json_t * json)
{
  mValid = true;

  json_t * pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
      mId = json_string_value(pValue);
      mAnnId = mId;
    }

  mValid &= !mId.empty();

  pValue = json_object_get(json, "features");

  std::vector< size_t > Required;
  size_t bits = 0;

  // Iterate of the array elements
  for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i)
    {
      Feature Feature;
      Feature.fromJSON(json_array_get(pValue, i));

      if (Feature.isValid())
        {
          mFeatureMap[Feature.getId()] = Feature;

          size_t required = Feature.bitsRequired();
          Required.push_back(required);
          bits += required;
        }
      else
        {
          mValid = false;
        }
    }

  Annotation::fromJSON(json);

  mBytes = ceil(std::max(bits/8.0, 4.0));

  std::map< std::string, Feature >::iterator it = mFeatureMap.begin();
  std::map< std::string, Feature >::iterator end = mFeatureMap.end();
  std::vector< size_t >::const_iterator itRequired = Required.begin();


  for (size_t start = 0; it != end; ++it, ++itRequired)
    {
       TraitData Data(*this);
       Data.setBits(start, start + *itRequired);
       start += *itRequired;

       it->second.setMask(Data);
    }
}

const std::string & Trait::getId() const
{
  return mId;
}

const bool & Trait::isValid() const
{
  return mValid;
}

size_t Trait::size() const
{
  return mBytes;
}

const Feature & Trait::getFeature(const std::string & id) const
{
  static Feature Missing;

  std::map< std::string, Feature >::const_iterator found = mFeatureMap.find(id);

  if (found != mFeatureMap.end())
    {
      return found->second;
    }

  return Missing;
}



