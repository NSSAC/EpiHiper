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
#include <set>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <string.h>
#include <jansson.h>

#include "traits/Trait.h"
#include "SimConfig.h"

// static
std::map< std::string, Trait > Trait::INSTANCES;
// static
Trait const * Trait::ActivityTrait;

// static
Trait const * Trait::EdgeTrait;

// static
Trait const * Trait::NodeTrait;

// static
void Trait::init()
{
  if (INSTANCES.empty())
    {
      Trait::ActivityTrait = &INSTANCES["activityTrait"];
      Trait::EdgeTrait = &INSTANCES["edgeTrait"];
      Trait::NodeTrait = &INSTANCES["nodeTrait"];
    }
}


// static
void Trait::load(const std::string & jsonFile)
{
  std::map< std::string, Trait > Traits;

  json_t * pRoot = SimConfig::loadJson(jsonFile, JSON_DECODE_INT_AS_REAL);

  if (pRoot == NULL)
    {
      return;
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
          INSTANCES[Trait.getId()] = Trait;
        }
    }

  json_decref(pRoot);

  return;
}

Trait::Trait()
  : Annotation()
  , mId()
  , mBytes(0)
  , mFeatures()
  , mFeatureMap()
  , mValid(false)
{}

Trait::~Trait() {}

Trait::Trait(const Trait & src)
  : Annotation(src)
  , mId(src.mId)
  , mBytes(src.mBytes)
  , mFeatures(src.mFeatures)
  , mFeatureMap()
  , mValid(src.mValid)
{
  std::vector< Feature >::iterator it = mFeatures.begin();
  std::vector< Feature >::iterator end = mFeatures.end();

  for (; it != end; ++it)
    mFeatureMap[it->getId()] = &*it;
}

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
          mFeatures.push_back(Feature);
          mFeatureMap[Feature.getId()] = &*mFeatures.rbegin();

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

  std::vector< Feature >::iterator it = mFeatures.begin();
  std::vector< Feature >::iterator end = mFeatures.end();
  std::vector< size_t >::const_iterator itRequired = Required.begin();


  for (size_t start = 0; it != end; ++it, ++itRequired)
    {
       TraitData Data(*this);
       Data.setBits(start, start + *itRequired);
       start += *itRequired;

       it->setMask(Data.to_ulong());
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

const Feature & Trait::operator[](const size_t & index) const
{
  static Feature Missing;

  if (index < mFeatures.size())
    {
      return mFeatures[index];
    }

  return Missing;
}

const Feature & Trait::operator[](const std::string & id) const
{
  static Feature Missing;

  std::map< std::string, Feature * >::const_iterator found = mFeatureMap.find(id);

  if (found != mFeatureMap.end())
    {
      return *found->second;
    }

  return Missing;
}

TraitData::base Trait::getDefault() const
{
  TraitData::base Default(0);

  std::vector< Feature >::const_iterator it = mFeatures.begin();
  std::vector< Feature >::const_iterator end = mFeatures.end();

  for (; it != end; ++it)
    {
      Default |= it->getDefault().getMask();
    }

  return Default;
}

bool Trait::fromString(const std::string & str, TraitData::base & data) const
{
  bool success = true;
  const char * ptr = str.c_str();

  std::set< size_t > FeaturesFound;
  size_t FeatureIndex;
  size_t EnumIndex;
  int Read;

  while (1 < sscanf(ptr, "%zu:%zu%n", &FeatureIndex, &EnumIndex, &Read))
    {
      const Feature & F = operator[](FeatureIndex - 1);
      const Enum & E = F[EnumIndex - 1];

      if (E.isValid())
        {
          TraitData::setValue(data, TraitData::value(F.getMask(), E.getMask()));
          FeaturesFound.insert(FeatureIndex);
        }
      else
        {
          success = false;
        }

      if (ptr[Read] != '|') break;

      ptr += Read + 1;
    }

  success &= (FeaturesFound.size() == mFeatures.size());

  return success;
}

std::string Trait::toString(TraitData::base & data) const
{
  std::ostringstream os;
  std::vector< Feature >::const_iterator it = mFeatures.begin();
  std::vector< Feature >::const_iterator end = mFeatures.end();
  bool FirstTime = true;

  for (size_t i = 1; it != end; ++it, ++i)
    {
      TraitData::value Value = TraitData::getValue(data, it->getMask());

      size_t FirstBit = TraitData(Value.first).firstBit();

      if (FirstTime)
        FirstTime = false;
      else
        os << "|";

      os << i << ":" << (Value.second >> FirstBit + 1);
    }

  return os.str();
}




