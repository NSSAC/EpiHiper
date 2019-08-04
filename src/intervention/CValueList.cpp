#include <jansson.h>

#include "CValueList.h"
#include "traits/CTrait.h"

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
  const CTrait * pTrait = NULL;
  const CFeature * pFeature = NULL;

  json_t * pValue = json_object_get(json, "boolean");
  json_t * pArray = NULL;

  if (json_is_array(pValue))
    {
      mType = Type::boolean;
      pArray = pValue;
    }

  if (pArray == NULL)
    {
      pValue = json_object_get(json, "number");

      if (json_is_array(pValue))
        {
          mType = Type::number;
          json_t * pArray = pValue;
        }
    }

  if (pArray == NULL)
    {
      pValue = json_object_get(json, "healthState");

      if (json_is_array(pValue))
        {
          mType = Type::healthState;
          json_t * pArray = pValue;
        }
    }

  if (pArray == NULL)
    {
      json_t * pValue = json_object_get(json, "trait");

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

      pValue = json_object_get(json, "feature");

      if (json_is_string(pValue))
        {
          pFeature = &pTrait->operator [](json_string_value(pValue));
        }

      if (pFeature == NULL)
        {
          mValid = false;
          return;
        }

      // Loop through the array elements
      pValue = json_object_get(json, "enum");

      if (json_is_array(pValue))
        {
          mType = Type::traitValue;
          json_t * pArray = pValue;
        }
    }

  if  (pArray == NULL)
    {
      mValid = false;
      return;
    }

  // Iterate of the array elements
  switch (mType)
  {
    case Type::boolean:
      for (size_t i = 0, imax = json_array_size(pArray); i < imax && mValid; ++i)
        {
          pValue = json_array_get(pArray, i);

          if (json_is_boolean(pValue))
            insert(json_is_true(pValue) ? true : false);
          else
            mValid = false;
        }
      break;

    case Type::number:
      for (size_t i = 0, imax = json_array_size(pArray); i < imax && mValid; ++i)
        {
          pValue = json_array_get(pArray, i);

          if (json_is_real(pValue))
            insert(json_real_value(pValue));
          else
            mValid = false;
        }
      break;

    case Type::healthState:
      for (size_t i = 0, imax = json_array_size(pArray); i < imax && mValid; ++i)
        {
          pValue = json_array_get(pArray, i);

          if (json_is_string(pValue))
            insert(CModel::stateToType(CModel::getState(json_string_value(pValue))));
          else
            mValid = false;
        }
      break;

    case Type::traitValue:
      for (size_t i = 0, imax = json_array_size(pArray); i < imax && mValid; ++i)
        {
          pValue = json_array_get(pArray, i);

          if (json_is_string(pValue))
            {
              const CEnum & Enum = pFeature->operator [](json_string_value(pValue));

              if (Enum.isValid())
                insert(CTraitData::value(pFeature->getMask(), Enum.getMask()));
              else
                mValid = false;
            }
          else
            mValid = false;
        }
      break;
  }
}


CValueList::CValueList(std::istream & is)
  : std::set< CValue >()
  , mType()
  , mValid(true)
{
  is.read(reinterpret_cast<char *>(&mType), sizeof(Type));
  size_t Size = 0;
  is.read(reinterpret_cast<char *>(&Size), sizeof(size_t));

  switch (mType)
  {
    case Type::boolean:
      for (size_t i = 0; i < Size; ++i)
        {
          bool Value;
          is.read(reinterpret_cast<char *>(&Value), sizeof(Value));
          insert(Value);
        }
      break;

    case Type::number:
      for (size_t i = 0; i < Size; ++i)
        {
          double Value;
          is.read(reinterpret_cast<char *>(&Value), sizeof(Value));
          insert(Value);
        }
      break;

    case Type::healthState:
      for (size_t i = 0; i < Size; ++i)
        {
          CModel::state_t Value;
          is.read(reinterpret_cast<char *>(&Value), sizeof(Value));
          insert(Value);
        }
      break;

    case Type::traitValue:
      for (size_t i = 0; i < Size; ++i)
        {
          CTraitData::value Value;
          is.read(reinterpret_cast<char *>(&Value), sizeof(Value));
          insert(Value);
        }
      break;
  }
}


bool CValueList::append(const CValue & value)
{
  if (value.getType() != mType) return false;

  if (mType == Type::traitValue
      && size() > 0
      && begin()->toTraitValue().first != value.toTraitValue().first) return false;

  insert(value);

  return true;
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
  *this = CValueList(json);
}

void CValueList::toBinary(std::ostream & os) const
{
  os.write(reinterpret_cast<const char *>(&mType), sizeof(Type));
  size_t Size = size();
  os.write(reinterpret_cast<const char *>(&Size), sizeof(size_t));

  const_iterator it = begin();
  const_iterator itEnd = end();

  switch (mType)
  {
    case Type::boolean:
      for (; it != itEnd; ++it)
        os.write(reinterpret_cast<const char *>(&it->toBoolean()), sizeof(bool));
      break;

    case Type::number:
      for (; it != itEnd; ++it)
        os.write(reinterpret_cast<const char *>(&it->toNumber()), sizeof(double));
      break;

    case Type::healthState:
      for (; it != itEnd; ++it)
        os.write(reinterpret_cast<const char *>(&it->toHealthState()), sizeof(CModel::state_t));
      break;

    case Type::traitValue:
      for (; it != itEnd; ++it)
        os.write(reinterpret_cast<const char *>(&it->toTraitValue()), sizeof(CTraitData::value));
      break;
  }
}

void CValueList::fromBinary(std::istream & is)
{
  *this = CValueList(is);
}


