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

#include "CFeature.h"

#include <cmath>
#include <iostream>
#include <jansson.h>


CFeature::CFeature()
  : CAnnotation()
  , mId()
  , mDefaultId()
  , mMask(0)
  , mEnums()
  , mEnumMap()
  , mValid(false)
{}

CFeature::CFeature(const CFeature & src)
  : CAnnotation(src)
  , mId(src.mId)
  , mMask(src.mMask)
  , mDefaultId(src.mDefaultId)
  , mEnums(src.mEnums)
  , mEnumMap()
  , mValid(src.mValid)
{
  updateEnumMap();
}

CFeature::~CFeature() {}

void CFeature::updateEnumMap()
{
  mEnumMap.clear();

  std::vector< CEnum >::iterator it = mEnums.begin();
  std::vector< CEnum >::iterator end = mEnums.end();

  for (; it != end; ++it)
    mEnumMap[it->getId()] = &*it;
}

void CFeature::fromJSON(const json_t * json)
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
      CEnum Enum;
      Enum.fromJSON(json_array_get(pValue, i));

      if (Enum.isValid())
        {
          Enum.setMask(CTraitData(i).to_ulong());
          mEnums.push_back(Enum);
        }
      else
        {
          mValid = false;
        }
    }

  pValue = json_object_get(json, "default");

  if (json_is_string(pValue))
    {
      mDefaultId = json_string_value(pValue);
    }

  updateEnumMap();

  mValid &= (mEnumMap.find(json_string_value(pValue)) != mEnumMap.end());

  CAnnotation::fromJSON(json);
}

const std::string & CFeature::getId() const
{
  return mId;
}

const bool & CFeature::isValid() const
{
  return mValid;
}


size_t CFeature::size() const
{
  return mEnumMap.size();
}

size_t CFeature::bitsRequired() const
{
  return ceil(log(mEnumMap.size())/log(2.0));
}

void CFeature::setMask(const CTraitData::base & mask)
{
  mMask = mask;
  size_t FirstBit = CTraitData(mMask).firstBit();

  std::vector< CEnum >::iterator it = mEnums.begin();
  std::vector< CEnum >::iterator end = mEnums.end();

  for (; it != end; ++it)
    {
      CTraitData::base Mask = it->getMask();
      Mask <<= FirstBit;
      it->setMask(Mask);
    }
}

const CTraitData::base & CFeature::getMask() const
{
  return mMask;
}

const CEnum & CFeature::operator[](const size_t & index) const
{
  static CEnum Missing;

  if (index < mEnums.size())
    {
      return mEnums[index];
    }

  return Missing;
}

const CEnum & CFeature::operator[](const std::string & id) const
{
  static CEnum Missing;

  std::map< std::string, CEnum * >::const_iterator found = mEnumMap.find(id);

  if (found != mEnumMap.end())
    {
      return *found->second;
    }

  return Missing;
}

const CEnum & CFeature::getDefault() const
{
  return operator[](mDefaultId);
}


