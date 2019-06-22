/*
 * Feature.cpp
 *
 *  Created on: Jun 21, 2019
 *      Author: shoops
 */

#include <cmath>
#include <jansson.h>
#include <iostream>

#include "TraitData.h"
#include "Feature.h"

Feature::Feature()
  : mName()
  , mDefault()
  , mMask()
  , mEnumMap()
  , mValid(false)
{}

Feature::Feature(const Feature & src)
  : mName(src.mName)
  , mMask(src.mMask)
  , mDefault(src.mDefault)
  , mEnumMap(src.mEnumMap)
  , mValid(src.mValid)
{}

Feature::~Feature() {}

void Feature::fromJSON(const json_t * json)
{
  /*
  "id": {"$ref": "#/definitions/uniqueId"},
  "enums": {"$ref": "#/definitions/featureEnums"},
  "default": {"$ref": "#/definitions/uniqueIdRef"}
  */

  json_t * pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
      mName = json_string_value(pValue);
    }

  pValue = json_object_get(json, "default");

    if (json_is_string(pValue))
      {
        mDefault = json_string_value(pValue);
      }

  pValue = json_object_get(json, "enums");

  // Iterate of the array elements
  for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i)
    {
      json_t * pEnum = json_object_get(json_array_get(pValue, i), "id");

      if (json_is_string(pEnum))
        {
          mEnumMap[json_string_value(pEnum)] = TraitData(i);
        }
    }

  mValid = !mName.empty() &&
           !mDefault.empty() &&
           mEnumMap.find(mDefault) != mEnumMap.end();
}

const std::string & Feature::getName() const
{
  return mName;
}

const bool & Feature::isValid() const
{
  return mValid;
}


size_t Feature::size() const
{
  return mEnumMap.size();
}

size_t Feature::bitsRequired() const
{
  return ceil(log(mEnumMap.size())/log(2.0));
}

void Feature::setMask(const TraitData & mask)
{
  mMask = mask;
  size_t FirstBit = mMask.firstBit();

  std::map< std::string, TraitData >::iterator it = mEnumMap.begin();
  std::map< std::string, TraitData >::iterator end = mEnumMap.end();

  for (; it != end; ++it)
    {
      it->second <<= FirstBit;
    }
}

const TraitData & Feature::getMask() const
{
  return mMask;
}

const TraitData & Feature::getEnum(const std::string & name) const
{
  static TraitData Missing;

  std::map< std::string, TraitData >::const_iterator found = mEnumMap.find(name);

  if (found != mEnumMap.end())
    {
      return found->second;
    }

  return Missing;
}




