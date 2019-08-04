#include <cstring>
#include <limits>
#include <jansson.h>

#include "intervention/CValue.h"
#include "traits/CTrait.h"

CValue::CValue(const Type & type)
  : mType(type)
  , mpValue(NULL)
  , mValid(true)
{
  createValue();
}


CValue::CValue(const bool & boolean)
  : mType(Type::boolean)
  , mpValue(NULL)
  , mValid(true)
{
  mpValue = new bool(boolean);
}

CValue::CValue(const double & number)
  : mType(Type::number)
  , mpValue(NULL)
  , mValid(true)
{
  mpValue = new double(number);
}

CValue::CValue(const CModel::state_t & healthState)
  : mType(Type::healthState)
  , mpValue(NULL)
  , mValid(true)
{
  mpValue = new CModel::state_t(healthState);
}

CValue::CValue(const CTraitData::value & traitValue)
  : mType(Type::traitValue)
  , mpValue(NULL)
  , mValid(true)
{
  mpValue = new CTraitData::value(traitValue);
}

CValue::CValue(const CValue & src)
  : mType(src.mType)
  , mpValue(NULL)
  , mValid(src.mValid)
{
  assignValue(src.mpValue);
}

CValue::CValue(const json_t * json)
  : mType()
  , mpValue(NULL)
  , mValid(true)
{
  json_t * pValue = json_object_get(json, "boolean");

  if (json_is_boolean(pValue))
    {
      mType = Type::boolean;
      mpValue = new bool(json_is_true(pValue) ? true : false);
      return;
    }

  pValue = json_object_get(json, "number");

  if (json_is_real(pValue))
    {
      mType = Type::number;
      mpValue = new double(json_real_value(pValue));
      return;
    }

  pValue = json_object_get(json, "healthState");

  if (json_is_string(pValue))
    {
      mType = Type::healthState;
      mpValue = new CModel::state_t(CModel::stateToType(CModel::getState(json_string_value(pValue))));
      return;
    }

  pValue = json_object_get(json, "trait");
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

  pValue = json_object_get(json, "feature");
  const CFeature * pFeature = NULL;

  if (json_is_string(pValue))
    {
      pFeature = &pTrait->operator [](json_string_value(pValue));
    }

  if (pFeature == NULL)
    {
      mValid = false;
      return;
    }

  pValue = json_object_get(json, "enum");
  const CEnum * pEnum = NULL;

  if (json_is_string(pValue))
    {
      pEnum = &pFeature->operator [](json_string_value(pValue));
    }

  if (pEnum == NULL)
    {
      mValid = false;
      return;
    }

  mType = Type::traitValue;
  mpValue = new CTraitData::value(pFeature->getMask(), pEnum->getMask());
}


CValue::CValue(std::istream & is)
  : mType()
  , mpValue(NULL)
  , mValid(true)
{
  is.read(reinterpret_cast<char *>(&mType), sizeof(Type));
  createValue();

  switch (mType)
  {
    case Type::boolean:
      is.read(reinterpret_cast<char *>(&mpValue), sizeof(bool));
      break;

    case Type::number:
      is.read(reinterpret_cast<char *>(&mpValue), sizeof(double));
      break;

    case Type::healthState:
      is.read(reinterpret_cast<char *>(&mpValue), sizeof(CModel::state_t));
      break;

    case Type::traitValue:
      is.read(reinterpret_cast<char *>(&mpValue), sizeof(CTraitData::value));
      break;

    default:
      mValid = false;
      break;
  }
}

// virtual
CValue::~CValue()
{
  destroyValue();
}

const bool & CValue::toBoolean() const
{
  return * static_cast< bool * >(mpValue);
}

const double & CValue::toNumber() const
{
  return * static_cast< double * >(mpValue);
}

const CModel::state_t & CValue::toHealthState() const
{
  return * static_cast< CModel::state_t * >(mpValue);
}

const CTraitData::value & CValue::toTraitValue() const
{
  return * static_cast< CTraitData::value * >(mpValue);
}

const CValue::Type & CValue::getType() const
{
  return mType;
}

const bool & CValue::isValid() const
{
  return mValid;
}

void CValue::fromJSON(const json_t * json)
{
  *this = CValue(json);
}

void CValue::toBinary(std::ostream & os) const
{
  os.write(reinterpret_cast<const char *>(&mType), sizeof(Type));

  switch (mType)
  {
    case Type::boolean:
      os.write(reinterpret_cast<const char *>(mpValue), sizeof(bool));
      break;

    case Type::number:
      os.write(reinterpret_cast<const char *>(mpValue), sizeof(double));
      break;

    case Type::healthState:
      os.write(reinterpret_cast<const char *>(mpValue), sizeof(CModel::state_t));
      break;

    case Type::traitValue:
      os.write(reinterpret_cast<const char *>(mpValue), sizeof(CTraitData::value));
      break;
  }
}

void CValue::fromBinary(std::istream & is)
{
  *this = CValue(is);
}

void CValue::createValue()
{
  if (mpValue != NULL) return;

  switch (mType)
  {
    case Type::boolean:
      mpValue = new bool(false);
      break;

    case Type::number:
      mpValue = new double(std::numeric_limits< double >::quiet_NaN());
      break;

    case Type::healthState:
      mpValue = new CModel::state_t(CModel::stateToType(&CModel::getInitialState()));
      break;

    case Type::traitValue:
      mpValue = new CTraitData::value(0, 0);
      break;
  }
}

void CValue::assignValue(const void * pValue)
{
  if (mpValue == NULL) createValue();

  switch (mType)
  {
    case Type::boolean:
      memcpy(mpValue, pValue, sizeof(bool));
      break;

    case Type::number:
      memcpy(mpValue, pValue, sizeof(double));
      break;

    case Type::healthState:
      memcpy(mpValue, pValue, sizeof(CModel::state_t));
      break;

    case Type::traitValue:
      memcpy(mpValue, pValue, sizeof(CTraitData::value));
      break;
  }
}

void CValue::destroyValue()
{
  if (mpValue != NULL) return;

  switch (mType)
  {
    case Type::boolean:
      delete (bool *) mpValue;
      break;

    case Type::number:
      delete (double *) mpValue;
      break;

    case Type::healthState:
      delete (CModel::state_t *) mpValue;
      break;

    case Type::traitValue:
      delete (CTraitData::value *) mpValue;
      break;
  }
}

bool CValue::operator<(const CValue & rhs) const
{
  if (mType != rhs.mType)
    return mType < rhs.mType;

  switch (mType)
  {
    case Type::boolean:
      return * static_cast< const bool * >(mpValue) < * static_cast< const bool * >(rhs.mpValue);
      break;

    case Type::number:
      return * static_cast< const double * >(mpValue) < * static_cast< const double * >(rhs.mpValue);
      break;

    case Type::healthState:
      return * static_cast< const CModel::state_t * >(mpValue) < * static_cast< const CModel::state_t * >(rhs.mpValue);
      break;

    case Type::traitValue:
      return * static_cast< const CTraitData::value * >(mpValue) < * static_cast< const CTraitData::value * >(rhs.mpValue);
      break;
  }

  return mpValue < rhs.mpValue;
}

bool CValue::operator==(const CValue & rhs) const
{
  if (mType != rhs.mType)
    return false;

  switch (mType)
  {
    case Type::boolean:
      return * static_cast< const bool * >(mpValue) == * static_cast< const bool * >(rhs.mpValue);
      break;

    case Type::number:
      return * static_cast< const double * >(mpValue) == * static_cast< const double * >(rhs.mpValue);
      break;

    case Type::healthState:
      return * static_cast< const CModel::state_t * >(mpValue) == * static_cast< const CModel::state_t * >(rhs.mpValue);
      break;

    case Type::traitValue:
      return * static_cast< const CTraitData::value * >(mpValue) == * static_cast< const CTraitData::value * >(rhs.mpValue);
      break;
  }

  return mpValue == rhs.mpValue;
}



