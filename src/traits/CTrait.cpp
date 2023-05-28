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

#include "traits/CTrait.h"
#include "utilities/CSimConfig.h"
#include "utilities/CLogger.h"

// static
void CTrait::init()
{
  // Initialize empty predefined traits
  if (INSTANCES.empty())
    {
      CTrait::ActivityTrait = &INSTANCES["activityTrait"];
      CTrait::EdgeTrait = &INSTANCES["edgeTrait"];
      CTrait::NodeTrait = &INSTANCES["nodeTrait"];
    }
}

// static 
const CTrait * CTrait::find(const std::string & id)
{
  std::map< std::string, CTrait >::const_iterator found = INSTANCES.find(id);

  if (found != INSTANCES.end())
    {
      return &found->second;
    }

  return NULL;
}

// static
void CTrait::load(const std::string & jsonFile)
{
  json_t * pRoot = CSimConfig::loadJson(jsonFile, JSON_DECODE_INT_AS_REAL);

  if (pRoot == NULL)
    {
      return;
    }

  load(pRoot);
  json_decref(pRoot);
}

// static 
void CTrait::load(const json_t * json)
{
  json_t * pTraits = json_object_get(json, "traits");

  // Iterate of the array elements
  for (size_t i = 0, imax = json_array_size(pTraits); i < imax; ++i)
    {
      json_t * pItem = json_array_get(pTraits, i);
      json_t * pValue = json_object_get(pItem, "id");

      if (!json_is_string(pValue))
        {
          CLogger::error("Trait: Invalid or missing value for 'id'.");
          return;
        }

      std::map< std::string, CTrait >::iterator found = INSTANCES.emplace(std::string(json_string_value(pValue)), CTrait()).first;

      found->second.fromJSON(pItem);
    }

  return;
}

// static 
void CTrait::loadSingle(const json_t * json)
{
  // Check whether we have an id object and load the corresponding trait object.
  json_t * pValue = json_object_get(json, "id");

  if (!json_is_string(pValue))
    {
      CLogger::error("Trait: Invalid or missing value for 'id'.");
      return;
    }

  std::map< std::string, CTrait >::iterator found = INSTANCES.emplace(std::string(json_string_value(pValue)), CTrait()).first;

  found->second.fromJSON(json);
}

void CTrait::updateFeatureMap()
{
  mFeatureMap.clear();

  std::vector< CFeature >::iterator it = mFeatures.begin();
  std::vector< CFeature >::iterator end = mFeatures.end();

  for (; it != end; ++it)
    mFeatureMap[it->getId()] = &*it;
}

CTrait::CTrait()
  : CAnnotation()
  , mId()
  , mBytes(0)
  , mFeatures()
  , mFeatureMap()
  , mTextEncoding()
  , mTextDecoding()
  , mValueEncoding()
  , mValid(false)
{}

CTrait::~CTrait() {}

CTrait::CTrait(const CTrait & src)
  : CAnnotation(src)
  , mId(src.mId)
  , mBytes(src.mBytes)
  , mFeatures(src.mFeatures)
  , mFeatureMap()
  , mTextEncoding(src.mTextEncoding)
  , mTextDecoding(src.mTextDecoding)
  , mValueEncoding(src.mValueEncoding)
  , mValid(src.mValid)
{
  updateFeatureMap();
}

CTrait & CTrait::operator=(const CTrait & rhs)
{
  if (this != &rhs)
    {
      CAnnotation::operator=(rhs);

      mId = rhs.mId;
      mBytes = rhs.mBytes;
      mFeatures = rhs.mFeatures;
      mTextEncoding = rhs.mTextEncoding;
      mTextDecoding = rhs.mTextDecoding;
      mValueEncoding = rhs.mValueEncoding;
      mValid = rhs.mValid;

      updateFeatureMap();
    }

  return *this;
}

void CTrait::fromJSON(const json_t * json)
{
  mValid = false; // DONE

  json_t * pValue = json_object_get(json, "id");

  if (mId.empty())
    {
      if (json_is_string(pValue))
        {
          mId = json_string_value(pValue);
          mAnnId = mId;
        }

      if (mId.empty())
        {
          CLogger::error("Trait: Invalid or missing value for 'id'.");
          return;
        }

      CAnnotation::fromJSON(json);
    }

  pValue = json_object_get(json, "features");

  // Iterate of the array elements
  for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i)
    {
      const CFeature * pFeature = operator[](json_array_get(pValue, i));

      // Augmenting or redefining a feature is not supported supported
      if (pFeature!= NULL)
        {
          CLogger::warn() << "Trait: Duplicate definition of '" << mId << "[" << pFeature->getId() << "]'.";
          continue;
        }

      CFeature Feature;
      Feature.fromJSON(json_array_get(pValue, i));

      if (Feature.isValid())
        {
          mFeatures.push_back(Feature);
        }
      else
        {
          CLogger::error() << "Trait: Invalid value for item '" << i << "'.";
          return;
        }
    }

  remap();
  mValid = true;
}

const std::string & CTrait::getId() const
{
  return mId;
}

const bool & CTrait::isValid() const
{
  return mValid;
}

size_t CTrait::size() const
{
  return mBytes;
}

const CFeature * CTrait::operator[](const size_t & index) const
{
  if (index < mFeatures.size())
    {
      return &mFeatures[index];
    }

  return NULL;
}

const CFeature * CTrait::operator[](const std::string & id) const
{
  std::map< std::string, CFeature * >::const_iterator found = mFeatureMap.find(id);

  if (found != mFeatureMap.end())
    {
      return found->second;
    }

  return NULL;
}

const CFeature * CTrait::operator[](const json_t * json) const
{
  json_t * pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
      return operator[](json_string_value(pValue));
    }

  return NULL;
}

CTraitData::base CTrait::getDefault() const
{
  CTraitData::base Default(0);

  std::vector< CFeature >::const_iterator it = mFeatures.begin();
  std::vector< CFeature >::const_iterator end = mFeatures.end();

  for (; it != end; ++it)
    if (it->getDefault() != NULL)
      {
        Default |= it->getDefault()->getMask();
      }

  return Default;
}

bool CTrait::fromString(const char * str, CTraitData::base & data) const
{
  std::map< std::string, CTraitData::base >::const_iterator found = mTextDecoding.find(str);

  if (found != mTextDecoding.end())
    {
      data = found->second;
      return true;
    }

  bool success = false;
  const char * ptr = str;

  std::set< size_t > FeaturesFound;
  size_t FeatureIndex;
  size_t EnumIndex;
  int Read;

  while (1 < sscanf(ptr, "%zu:%zu%n", &FeatureIndex, &EnumIndex, &Read))
    {
      const CFeature * pF = operator[](FeatureIndex - 1);
      const CEnum * pE = (pF != NULL) ? pF->operator[](EnumIndex - 1) : NULL;

      if (pE != NULL
          && pE->isValid())
        {
          CTraitData::setValue(data, CTraitData::value(pF->getMask(), pE->getMask()));
          FeaturesFound.insert(FeatureIndex);
          success = true;
        }
      else
        {
          success = false;
          break;
        }

      if (ptr[Read] != '|')
        break;

      ptr += Read + 1;
    }

  if (!success)
    CLogger::error() << "CTrait: Invalid trait encoding '" << str << "'.";

  mTextDecoding[str] = data;

  return success;
}

std::string CTrait::toString(const CTraitData::base & data) const
{
  std::map< CTraitData::base, std::string >::const_iterator found = mTextEncoding.find(data);

  if (found != mTextEncoding.end())
    {
      return found->second;
    }

  std::ostringstream os;
  std::vector< CFeature >::const_iterator it = mFeatures.begin();
  std::vector< CFeature >::const_iterator end = mFeatures.end();
  bool FirstTime = true;

  for (size_t i = 1; it != end; ++it, ++i)
    {
      CTraitData::value Value = CTraitData::getValue(data, it->getMask());

      size_t FirstBit = CTraitData(Value.first).firstBit();

      if (FirstTime)
        FirstTime = false;
      else
        os << "|";

      os << i << ":" << (Value.second >> FirstBit) + 1;
    }

  mTextEncoding[data] = os.str();
  
  return os.str();
}

std::string CTrait::toString(const CTraitData::value & value) const
{
  std::map< CTraitData::value, std::string >::const_iterator found = mValueEncoding.find(value);

  if (found != mValueEncoding.end())
    {
      return found->second;
    }

  std::ostringstream os;
  std::vector< CFeature >::const_iterator it = mFeatures.begin();
  std::vector< CFeature >::const_iterator end = mFeatures.end();
  
  for (size_t i = 1; it != end; ++it, ++i)
    if (it->getMask() == value.first)
      {
        size_t FirstBit = CTraitData(value.first).firstBit();
        os << it->getId() << ":" << it->operator[](value.second >> FirstBit)->getId();
        break;
      }

  mValueEncoding[value] = os.str();
  
  return os.str();
}

const CFeature * CTrait::addFeature(const CFeature & feature)
{
  if (!feature.isValid())
    return NULL;

  mFeatures.push_back(feature);

  remap();

  return &*mFeatures.rbegin();
}

void CTrait::remap()
{
  std::vector< size_t > Required;
  size_t bits = 0;

  std::vector< CFeature >::iterator itFeature = mFeatures.begin();
  std::vector< CFeature >::iterator endFeature = mFeatures.end();

  // Iterate of the array elements
  for (; itFeature != endFeature; ++itFeature)
    {
      size_t required = itFeature->bitsRequired();
      Required.push_back(required);
      bits += required;
    }

  updateFeatureMap();

  mBytes = ceil(std::max(bits / 8.0, 4.0));

  itFeature = mFeatures.begin();
  std::vector< size_t >::const_iterator itRequired = Required.begin();

  for (size_t start = 0; itFeature != endFeature; ++itFeature, ++itRequired)
    {
      CTraitData Data(*this);
      Data.setBits(start, start + *itRequired);
      start += *itRequired;

      itFeature->setMask(Data.to_ulong());
    }
}
