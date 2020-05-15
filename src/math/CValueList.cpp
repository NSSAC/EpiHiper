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

#include "math/CValueList.h"

#include <jansson.h>

#include "traits/CTrait.h"
#include "utilities/CLogger.h"

CValueList::CValueList(const Type & type)
  : std::set< CValue >()
  , mType(type)
  , mValid(true)
{}

CValueList::CValueList(const CValueList & src)
  : std::set< CValue >(src)
  , mType(src.mType)
  , mValid(src.mValid)
{}

CValueList::CValueList(const json_t * json)
  : std::set< CValue >()
  , mType()
  , mValid(true)
{
  fromJSON(json);
}

CValueList::CValueList(std::istream & is)
  : std::set< CValue >()
  , mType()
  , mValid(true)
{
  fromBinary(is);
}

bool CValueList::append(const CValue & value)
{
  if (value.getType() != mType)
    return false;

  if (mType == Type::traitValue
      && size() > 0
      && begin()->toTraitValue().first != value.toTraitValue().first)
    return false;

  std::set< CValue >::insert(value);

  return true;
}

bool CValueList::contains(const CValueInterface & value) const
{
  if (value.getType() == CValueInterface::Type::traitData 
      && !empty())
    return contains(CValue(CTraitData::getValue(value.toTraitData(), std::set< CValue >::begin()->toTraitValue().first)));

  if (value.getType() != mType)
    return false;

  const CValue * pValue = reinterpret_cast< const CValue * >(&value);

  return (find(*pValue) != end());
}

// virtual
CValueList::~CValueList()
{}

CValueList::const_iterator CValueList::begin() const
{
  return std::set< CValue >::begin();
}

CValueList::const_iterator CValueList::end() const
{
  return std::set< CValue >::end();
}

const CValueList::Type & CValueList::getType() const
{
  return mType;
}

size_t CValueList::size() const
{
  return std::set< CValue >::size();
}

const bool & CValueList::isValid() const
{
  return mValid;
}

void CValueList::fromJSON(const json_t * json)
{
  /*
  "valueList": {
      "$id": "#valueList",
      "description": "A list of values used in comparisons and assignments.",
      "type": "object",
      "required": ["valueList"],
      "properties": {
        "valueList": {
          "oneOf": [
            {
              "type": "object",
              "required": ["boolean"],
              "properties": {
                "boolean": {
                  "type": "array",
                  "items": {"type": "boolean"}
                }
              }
            },
            {
              "type": "object",
              "required": ["number"],
              "properties": {
                "number": {
                  "type": "array",
                  "items": {"$ref": "#/definitions/nonNegativeNumber"}
                }
              }
            },
            {
              "type": "object",
              "required": ["healthState"],
              "properties": {
                "healthState": {
                  "type": "array",
                  "items": {"$ref": "#/definitions/uniqueIdRef"}
                }
              }
            },
            {
              "type": "object",
              "required": [
                "trait",
                "feature",
                "enum"
              ],
              "properties": {
                "trait": {
                  "description": "",
                  "$ref": "#/definitions/uniqueIdRef"
                },
                "feature": {
                  "description": "",
                  "$ref": "#/definitions/uniqueIdRef"
                },
                "enum": {
                  "type": "array",
                  "items": {"$ref": "#/definitions/uniqueIdRef"}
                }
              }
            }
          ]
        }
      }
    }
    */

  const json_t * pValueList = json_object_get(json, "valueList");

  if (pValueList == NULL)
    pValueList = json;

  mValid = false; // DONE
  const CTrait * pTrait = NULL;
  const CFeature * pFeature = NULL;

  json_t * pValue = json_object_get(pValueList, "boolean");
  json_t * pArray = NULL;

  if (json_is_array(pValue))
    {
      mType = Type::boolean;
      pArray = pValue;
    }

  if (pArray == NULL)
    {
      pValue = json_object_get(pValueList, "number");

      if (json_is_array(pValue))
        {
          mType = Type::number;
          pArray = pValue;
        }
    }

  if (pArray == NULL)
    {
      pValue = json_object_get(pValueList, "healthState");

      if (json_is_array(pValue))
        {
          mType = Type::id;
          pArray = pValue;
        }
    }

  if (pArray == NULL)
    {
      pValue = json_object_get(pValueList, "enum");

      if (json_is_array(pValue))
        {
          mType = Type::traitValue;
          pArray = pValue;
        }
    }

  if (pArray == NULL)
    {
      CLogger::error("Value list: Value type cannot be determined.");
      return;
    }

  if (mType == Type::traitValue)
    {
      json_t * pValue = json_object_get(pValueList, "trait");

      if (json_is_string(pValue))
        {
          std::map< std::string, CTrait >::const_iterator found = CTrait::INSTANCES.find(json_string_value(pValue));

          if (found != CTrait::INSTANCES.end())
            pTrait = &found->second;

          if (pTrait == NULL)
            {
              CLogger::error() << "Value list: Invalid trait '" << json_string_value(pValue) << "'.";
              return;
            }
        }
      else
        {
          CLogger::error("Value list: Missing trait.");
          return;
        }

      pValue = json_object_get(pValueList, "feature");

      if (json_is_string(pValue))
        {
          pFeature = pTrait->operator[](json_string_value(pValue));

          if (pFeature == NULL)
            {
              CLogger::error() << "Value list: Invalid feature '" << json_string_value(pValue) << "' for trait '" << pTrait->getId() << "'.";
              return;
            }
        }
      else
        {
          CLogger::error() << "Value list: Missing feature for trait '" << pTrait->getId() << "'.";
          return;
        }
    }

  // Iterate of the array elements
  switch (mType)
    {
    case Type::boolean:
      for (size_t i = 0, imax = json_array_size(pArray); i < imax; ++i)
        {
          pValue = json_array_get(pArray, i);

          if (json_is_boolean(pValue))
            std::set< CValue >::insert(json_is_true(pValue) ? true : false);
          else
            {
              CLogger::error() << "Value list (Boolean): Invalid value for item '" << i << "'.";
              return;
            }
        }
      break;

    case Type::number:
      for (size_t i = 0, imax = json_array_size(pArray); i < imax; ++i)
        {
          pValue = json_array_get(pArray, i);

          if (json_is_real(pValue))
            std::set< CValue >::insert(json_real_value(pValue));
          else
            {
              CLogger::error() << "Value list (number): Invalid value for item '" << i << "'.";
              return;
            }
        }
      break;

    case Type::integer:
      for (size_t i = 0, imax = json_array_size(pArray); i < imax; ++i)
        {
          pValue = json_array_get(pArray, i);

          if (json_is_integer(pValue))
            std::set< CValue >::insert((int) json_integer_value(pValue));
          else
            {
              CLogger::error() << "Value list (integer): Invalid value for item '" << i << "'.";
              return;
            }
        }
      break;

    case Type::id:
      for (size_t i = 0, imax = json_array_size(pArray); i < imax; ++i)
        {
          pValue = json_array_get(pArray, i);

          if (json_is_string(pValue))
            {
              mType = Type::id;

              const CHealthState * pHealthState = CModel::GetState(json_string_value(pValue));

              if (pHealthState == NULL)
                {
                  CLogger::error() << "Value list (health state): Invalid value for item '" << i << "'.";
                  return;
                }

              std::set< CValue >::insert(CModel::StateToType(pHealthState));
            }
          else
            {
              CLogger::error() << "Value list (health state): Invalid value for item '" << i << "'.";
              return;
            }
        }
      break;

    case Type::traitValue:
      for (size_t i = 0, imax = json_array_size(pArray); i < imax; ++i)
        {
          pValue = json_array_get(pArray, i);

          if (json_is_string(pValue))
            {
              const CEnum * pEnum = pFeature->operator[](json_string_value(pValue));

              if (pEnum == NULL)
                {
                  CLogger::error() << "Value list: Invalid enum '" << json_string_value(pValue) << "' for feature '" << pFeature->getId() << "' for trait '" << pTrait->getId() << "'.";
                  return;
                }
            }
          else
            {
              CLogger::error() << "Value list: Missing enum for item '" << i << "' for feature '" << pFeature->getId() << "' for trait '" << pTrait->getId() << "'.";
              return;
            }
        }
      break;

    case Type::string:
    case Type::traitData:
      break;
    }

  mValid = true;
}

void CValueList::toBinary(std::ostream & os) const
{
  os.write(reinterpret_cast< const char * >(&mType), sizeof(Type));
  size_t Size = size();
  os.write(reinterpret_cast< const char * >(&Size), sizeof(size_t));

  const_iterator it = begin();
  const_iterator itEnd = end();

  switch (mType)
    {
    case Type::boolean:
      for (; it != itEnd; ++it)
        os.write(reinterpret_cast< const char * >(&it->toBoolean()), sizeof(bool));
      break;

    case Type::number:
      for (; it != itEnd; ++it)
        os.write(reinterpret_cast< const char * >(&it->toNumber()), sizeof(double));
      break;

    case Type::integer:
      for (; it != itEnd; ++it)
        os.write(reinterpret_cast< const char * >(&it->toInteger()), sizeof(int));
      break;

    case Type::id:
      for (; it != itEnd; ++it)
        os.write(reinterpret_cast< const char * >(&it->toId()), sizeof(size_t));
      break;

    case Type::traitValue:
      for (; it != itEnd; ++it)
        os.write(reinterpret_cast< const char * >(&it->toTraitValue()), sizeof(CTraitData::value));
      break;

    case Type::string:
    case Type::traitData:
      break;
    }
}

void CValueList::fromBinary(std::istream & is)
{
  is.read(reinterpret_cast< char * >(&mType), sizeof(Type));
  size_t Size = 0;
  is.read(reinterpret_cast< char * >(&Size), sizeof(size_t));

  switch (mType)
    {
    case Type::boolean:
      for (size_t i = 0; i < Size; ++i)
        {
          bool Value;
          is.read(reinterpret_cast< char * >(&Value), sizeof(bool));
          std::set< CValue >::insert(Value);
        }
      break;

    case Type::number:
      for (size_t i = 0; i < Size; ++i)
        {
          double Value;
          is.read(reinterpret_cast< char * >(&Value), sizeof(double));
          std::set< CValue >::insert(Value);
        }
      break;

    case Type::integer:
      for (size_t i = 0; i < Size; ++i)
        {
          int Value;
          is.read(reinterpret_cast< char * >(&Value), sizeof(int));
          std::set< CValue >::insert(Value);
        }
      break;

    case Type::id:
      for (size_t i = 0; i < Size; ++i)
        {
          size_t Value;
          is.read(reinterpret_cast< char * >(&Value), sizeof(size_t));
          std::set< CValue >::insert(Value);
        }
      break;

    case Type::traitValue:
      for (size_t i = 0; i < Size; ++i)
        {
          CTraitData::value Value;
          is.read(reinterpret_cast< char * >(&Value), sizeof(CTraitData::value));
          std::set< CValue >::insert(Value);
        }
      break;
      
    case Type::string:
    case Type::traitData:
      break;
    }
}

CValueList::iterator CValueList::insert(CValueList::iterator position, const CValueList::value_type & val)
{
  return std::set< CValue >::insert(position, val);
}
