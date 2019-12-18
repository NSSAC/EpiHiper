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

#include "math/CValue.h"

#include <cstring>
#include <limits>
#include <jansson.h>

#include "traits/CTrait.h"

CValue::CValue(const bool & boolean)
  : CValueInterface(Type::boolean, createValue(Type::boolean))
  , mValid(true)
{
  assignValue(&boolean);
}

CValue::CValue(const double & number)
  : CValueInterface(Type::number, createValue(Type::number))
  , mValid(true)
{
  assignValue(&number);
}

CValue::CValue(const CTraitData::value & traitValue)
  : CValueInterface(Type::traitValue, createValue(Type::traitValue))
  , mValid(true)
{
  assignValue(&traitValue);
}

CValue::CValue(const std::string & str)
  : CValueInterface(Type::string, createValue(Type::string))
  , mValid(true)
{
  assignValue(&str);
}

CValue::CValue(const size_t & id)
  : CValueInterface(Type::id, createValue(Type::id))
  , mValid(true)
{
  assignValue(&id);
}


CValue::CValue(const CValue & src)
  : CValueInterface(src.mType, createValue(src.mType))
  , mValid(src.mValid)
{
  assignValue(src.mpValue);
}

CValue::CValue(const json_t * json)
  : CValueInterface(Type::boolean, NULL)
  , mValid(true)
{
  fromJSON(json);
}

// virtual
CValue::~CValue()
{
  destroyValue();
}

// virtual
CValueInterface * CValue::copy() const
{
  return new CValue(*this);
}

void CValue::fromJSON(const json_t * json)
{
  if (json == NULL)
    {
      mValid = false;
      return;
    }

  const json_t * pRoot = json_object_get(json, "value");

  if (pRoot == NULL)
    {
      pRoot = json;
    }

  json_t * pValue = json_object_get(pRoot, "boolean");

  if (json_is_boolean(pValue))
    {
      mType = Type::boolean;
      mpValue = new bool(json_is_true(pValue) ? true : false);
      mValid = true;
      return;
    }

  pValue = json_object_get(pRoot, "number");

  if (json_is_real(pValue))
    {
      mType = Type::number;
      mpValue = new double(json_real_value(pValue));
      mValid = true;
      return;
    }

  pValue = json_object_get(pRoot, "healthState");

  if (json_is_string(pValue))
    {
      mType = Type::id;
      mpValue = new CModel::state_t(CModel::stateToType(CModel::getState(json_string_value(pValue))));
      mValid = true;
      return;
    }

  pValue = json_object_get(pRoot, "trait");
  const CTrait * pTrait = NULL;

  if (json_is_string(pValue))
    {
      std::map< std::string, CTrait >::const_iterator found = CTrait::INSTANCES.find(json_string_value(pValue));

      if (found != CTrait::INSTANCES.end())
        pTrait = &found->second;
    }

  if (pTrait == NULL)
    {
      mValid = false;
      return;
    }

  pValue = json_object_get(pRoot, "feature");
  const CFeature * pFeature = NULL;

  if (json_is_string(pValue))
    {
      pFeature = pTrait->operator [](json_string_value(pValue));
    }

  if (pFeature == NULL)
    {
      pFeature = const_cast< CTrait * >(pTrait)->addFeature(CFeature(json_string_value(pValue)));

      if (pFeature == NULL)
        {
          mValid = false;
          return;
        }
    }

  pValue = json_object_get(pRoot, "enum");
  const CEnum * pEnum = NULL;

  if (json_is_string(pValue))
    {
      pEnum = pFeature->operator [](json_string_value(pValue));
    }

  if (pEnum == NULL)
    {
      pEnum = const_cast< CFeature * >(pFeature)->addEnum(CEnum(json_string_value(pValue)));

      if (pEnum == NULL)
        {
          mValid = false;
          return;
        }

      const_cast< CTrait * >(pTrait)->remap();
    }

  mType = Type::traitValue;
  mpValue = new CTraitData::value(pFeature->getMask(), pEnum->getMask());
  mValid = true;
}

const bool & CValue::isValid() const
{
  return mValid;
}

void CValue::toBinary(std::ostream & os) const
{
  os.write(reinterpret_cast<const char *>(&mType), sizeof(size_t));

  switch (mType)
  {
    case Type::boolean:
      os.write(reinterpret_cast<const char *>(mpValue), sizeof(bool));
      break;

    case Type::number:
      os.write(reinterpret_cast<const char *>(mpValue), sizeof(double));
      break;

    case Type::traitData:
      os.write(reinterpret_cast<const char *>(mpValue), sizeof(CTraitData::base));
      break;


    case Type::traitValue:
      os.write(reinterpret_cast<const char *>(static_cast< const CTraitData::value * >(mpValue)->first), sizeof(CTraitData::base));
      os.write(reinterpret_cast<const char *>(static_cast< const CTraitData::value * >(mpValue)->second), sizeof(CTraitData::base));
      break;

    case Type::string:
      {
        size_t length = static_cast< const std::string * >(mpValue)->length();
        os.write(reinterpret_cast<const char *>(&length), sizeof(size_t));
        os.write(static_cast< const std::string * >(mpValue)->c_str(), length);
      }
      break;

    case Type::id:
      os.write(reinterpret_cast<const char *>(mpValue), sizeof(size_t));
      break;
  }
}

void CValue::fromBinary(std::istream & is)
{
  destroyValue();

  is.read(reinterpret_cast<char *>(&mType), sizeof(size_t));
  mpValue = createValue(mType);

  switch (mType)
  {
    case Type::boolean:
      is.read(reinterpret_cast<char *>(mpValue), sizeof(bool));
      break;

    case Type::number:
      is.read(reinterpret_cast<char *>(mpValue), sizeof(double));
      break;

    case Type::traitData:
      is.read(reinterpret_cast<char *>(mpValue), sizeof(CTraitData::base));
      break;


    case Type::traitValue:
      is.read(reinterpret_cast<char *>(static_cast< CTraitData::value * >(mpValue)->first), sizeof(CTraitData::base));
      is.read(reinterpret_cast<char *>(static_cast< CTraitData::value * >(mpValue)->second), sizeof(CTraitData::base));
      break;

    case Type::string:
      {
        size_t length = static_cast< std::string * >(mpValue)->length();
        is.read(reinterpret_cast<char *>(&length), sizeof(size_t));
        char str[length + 1];
        is.read(str, length);
        str[length] = 0x0;
        *static_cast< std::string * >(mpValue) = str;
      }
      break;

    case Type::id:
      is.read(reinterpret_cast<char *>(mpValue), sizeof(size_t));
      break;
  }
}

// static
void * CValue::createValue(const CValue::Type & type)
{
  switch (type)
  {
    case Type::boolean:
      return new bool(false);
      break;

    case Type::number:
      return new double(std::numeric_limits< double >::quiet_NaN());
      break;

    case Type::traitData:
      return new CTraitData::base();
      break;

    case Type::traitValue:
      return new CTraitData::value(0, 0);
      break;

    case Type::string:
      return new std::string();
      break;

    case Type::id:
      return new size_t();
      break;
  }

  return NULL;
}

void CValue::assignValue(const void * pValue)
{
  switch (mType)
  {
    case Type::boolean:
      *static_cast< bool * >(mpValue) = *static_cast< const bool * >(pValue);
      break;

    case Type::number:
      *static_cast< double * >(mpValue) = *static_cast< const double * >(pValue);
      break;

    case Type::traitData:
      *static_cast< CTraitData::base * >(mpValue) = *static_cast< const CTraitData::base * >(pValue);
      break;

    case Type::traitValue:
      *static_cast< CTraitData::value * >(mpValue) = *static_cast< const CTraitData::value * >(pValue);
      break;

    case Type::string:
      *static_cast< std::string * >(mpValue) = *static_cast< const std::string * >(pValue);
      break;

    case Type::id:
      *static_cast< size_t * >(mpValue) = *static_cast< const size_t * >(pValue);
      break;
  }
}

void CValue::destroyValue()
{
  if (mpValue == NULL) return;

  switch (mType)
  {
    case Type::boolean:
      delete static_cast< bool * >(mpValue);
      break;

    case Type::number:
      delete static_cast< double * >(mpValue);
      break;

    case Type::traitData:
      delete static_cast< CTraitData::base * >(mpValue);
      break;

    case Type::traitValue:
      delete static_cast< CTraitData::value * >(mpValue);
      break;

    case Type::string:
      delete static_cast< std::string * >(mpValue);
      break;

    case Type::id:
      delete static_cast< size_t * >(mpValue);
      break;
  }
}



