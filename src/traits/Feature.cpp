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
 * Feature.cpp
 *
 *  Created on: Jun 21, 2019
 *      Author: shoops
 */

#include <cmath>
#include <iostream>
#include <jansson.h>

#include "Feature.h"

Feature::Feature()
  : Annotation()
  , mId()
  , mDefault()
  , mMask()
  , mEnumMap()
  , mValid(false)
{}

Feature::Feature(const Feature & src)
  : Annotation(src)
  , mId(src.mId)
  , mMask(src.mMask)
  , mDefault(src.mDefault)
  , mEnumMap(src.mEnumMap)
  , mValid(src.mValid)
{}

Feature::~Feature() {}

void Feature::fromJSON(const json_t * json)
{
  mValid = true;

  json_t * pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
      mId = json_string_value(pValue);
      mAnnId = mId;
    }

  mValid &= !mId.empty();

  pValue = json_object_get(json, "enums");

  // Iterate of the array elements
  for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i)
    {
      Enum Enum;
      Enum.fromJSON(json_array_get(pValue, i));

      if (Enum.isValid())
        {
          Enum.setMask(TraitData(i));
          mEnumMap[Enum.getId()] = Enum;
        }
      else
        {
          mValid = false;
        }
    }

  pValue = json_object_get(json, "default");

  if (json_is_string(pValue))
    {
      mDefault = json_string_value(pValue);
    }

  mValid &= !mDefault.empty() &&
             mEnumMap.find(mDefault) != mEnumMap.end();

  Annotation::fromJSON(json);
}

const std::string & Feature::getId() const
{
  return mId;
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

  std::map< std::string, Enum >::iterator it = mEnumMap.begin();
  std::map< std::string, Enum >::iterator end = mEnumMap.end();

  for (; it != end; ++it)
    {
      TraitData Mask = it->second.getMask();
      Mask <<= FirstBit;
      it->second.setMask(Mask);
    }
}

const TraitData & Feature::getMask() const
{
  return mMask;
}

const Enum & Feature::getEnum(const std::string & id) const
{
  static Enum Missing;

  std::map< std::string, Enum >::const_iterator found = mEnumMap.find(id);

  if (found != mEnumMap.end())
    {
      return found->second;
    }

  return Missing;
}




