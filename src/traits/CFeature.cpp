// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
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

#include "traits/CFeature.h"
#include "utilities/CLogger.h"

CFeature::CFeature(const std::string & id)
  : CAnnotation()
  , mId(id)
  , mDefaultId("notSet")
  , mMask(0)
  , mEnums()
  , mEnumMap()
  , mValid(!id.empty())
{
  if (mValid)
    {
      mAnnId = mId;
      addEnum(CEnum(mDefaultId));
    }
}

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
  mValid = false; // DONE

  json_t * pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
      mId = json_string_value(pValue);
      mAnnId = mId;
    }

  if (mId.empty())
    {
      CLogger::error("Feature: Invalid or missing 'id'.");
      return;
    }

  CAnnotation::fromJSON(json);

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
          CLogger::error() << "Feature: Invalid value for item '" << i << "'.";
          return;
        }
    }

  pValue = json_object_get(json, "default");

  if (json_is_string(pValue))
    {
      mDefaultId = json_string_value(pValue);
    }
  else
    {
      mDefaultId = "notSet";

      if (operator[]("notSet") == NULL)
        addEnum(CEnum("notSet"));
    }

  updateEnumMap();

  if (mEnumMap.find(mDefaultId) == mEnumMap.end())
    {
      CLogger::error() << "Feature: Invalid default '" << mDefaultId << "'.";
      return;
    }

  mValid = true;
}

void CFeature::augment(const json_t * json)
{
  CFeature Augment;
  Augment.fromJSON(json);

  if (!Augment.isValid())
    {
      mValid = false; // DONE
      return;
    }

  // Annotation is overwritten
  CAnnotation::fromJSON(json);

  std::vector< CEnum >::const_iterator it = Augment.mEnums.begin();
  std::vector< CEnum >::const_iterator end = Augment.mEnums.end();

  // Add all newly defined enums.
  for (; it != end; ++it)
    {
      if (it->isValid()
          && it->getId() != "notSet"
          && operator[](it->getId()) == NULL)
        {
          addEnum(*it);
        }
    }

  // The default is overwritten if it is defined in the augmentation.
  if (Augment.mDefaultId != "notSet")
    {
      mDefaultId = Augment.mDefaultId;
    }
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
  return std::max(1.0, ceil(log(mEnumMap.size()) / log(2.0)));
}

void CFeature::setMask(const CTraitData::base & mask)
{
  mMask = mask;
  size_t FirstBit = CTraitData(mMask).firstBit();

  std::vector< CEnum >::iterator it = mEnums.begin();
  std::vector< CEnum >::iterator end = mEnums.end();

  for (int i = 0; it != end; ++it, ++i)
    {
      CTraitData::base Mask = CTraitData(i).to_ulong();
      Mask <<= FirstBit;
      it->setMask(Mask);
    }
}

const CTraitData::base & CFeature::getMask() const
{
  return mMask;
}

const CEnum * CFeature::operator[](const size_t & index) const
{
  if (index < mEnums.size())
    {
      return &mEnums[index];
    }

  return NULL;
}

const CEnum * CFeature::operator[](const std::string & id) const
{
  std::map< std::string, CEnum * >::const_iterator found = mEnumMap.find(id);

  if (found != mEnumMap.end())
    {
      return found->second;
    }

  return NULL;
}

const CEnum * CFeature::getDefault() const
{
  return operator[](mDefaultId);
}

const CEnum * CFeature::addEnum(const CEnum & enumeration)
{
  if (!enumeration.isValid())
    return NULL;

  mEnums.push_back(enumeration);

  updateEnumMap();

  return &*mEnums.rbegin();
}
