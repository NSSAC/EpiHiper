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

#include "math/CValueInterface.h"
#include "utilities/CCommunicate.h"

CValueInterface::CValueInterface(const CValueInterface::Type & type, void * pValue)
  : mType(type)
  , mpValue(pValue)
{}

CValueInterface::CValueInterface(const CValueInterface & src)
  : mType(src.mType)
  , mpValue(src.mpValue)
{}

CValueInterface::CValueInterface(bool & boolean)
  : mType(Type::boolean)
  , mpValue(&boolean)
{}

CValueInterface::CValueInterface(double & number)
  : mType(Type::number)
  , mpValue(&number)
{}

CValueInterface::CValueInterface(size_t & id)
  : mType(Type::id)
  , mpValue(&id)
{}

CValueInterface::CValueInterface(CTraitData::base & traitData)
  : mType(Type::traitData)
  , mpValue(&traitData)
{}

CValueInterface::CValueInterface(CTraitData::value & traitValue)
  : mType(Type::traitValue)
  , mpValue(&traitValue)
{}

CValueInterface::CValueInterface(std::string & str)
  : mType(Type::string)
  , mpValue(&str)
{}

// virtual
CValueInterface::~CValueInterface()
{}

const bool & CValueInterface::toBoolean() const
{
  return * static_cast< bool * >(mpValue);
}

// virtual
CValueInterface * CValueInterface::copy() const
{
  return new CValueInterface(*this);
}

const double & CValueInterface::toNumber() const
{
  return * static_cast< double * >(mpValue);
}

const size_t & CValueInterface::toId() const
{
  return * static_cast< size_t * >(mpValue);
}

const CTraitData::base & CValueInterface::toTraitData() const
{
  return * static_cast< CTraitData::base * >(mpValue);
}

const CTraitData::value & CValueInterface::toTraitValue() const
{
  return * static_cast< CTraitData::value * >(mpValue);
}

const std::string & CValueInterface::toString() const
{
  return * static_cast< std::string * >(mpValue);
}

const CValueInterface::Type & CValueInterface::getType() const
{
  return mType;
}

void CValueInterface::toBinary(std::ostream & os) const
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
    case Type::id:
      os.write(reinterpret_cast<const char *>(mpValue), sizeof(size_t));
      break;

    case Type::traitData:
      os.write(reinterpret_cast<const char *>(mpValue), sizeof(CTraitData::base));
      break;

    case Type::traitValue:
      os.write(reinterpret_cast<const char *>(mpValue), sizeof(CTraitData::value));
      break;

    case Type::string:
      {
        std::string * pStr = static_cast< std::string * >(mpValue);
        size_t Size = pStr->length();
        os.write(reinterpret_cast<const char *>(&Size), sizeof(size_t));
        os.write(reinterpret_cast<const char *>(pStr->c_str()), Size * sizeof(char));
      }
      break;
  }
}

void CValueInterface::fromBinary(std::istream & is)
{
  is.read(reinterpret_cast<char *>(&mType), sizeof(Type));

  switch (mType)
  {
    case Type::boolean:
      is.read(reinterpret_cast<char *>(&mpValue), sizeof(bool));
      break;

    case Type::number:
      is.read(reinterpret_cast<char *>(&mpValue), sizeof(double));
      break;

    case Type::healthState:
    case Type::id:
      is.read(reinterpret_cast<char *>(&mpValue), sizeof(size_t));
      break;

    case Type::traitData:
      is.read(reinterpret_cast<char *>(&mpValue), sizeof(CTraitData::base));
      break;

    case Type::traitValue:
      is.read(reinterpret_cast<char *>(&mpValue), sizeof(CTraitData::value));
      break;

    case Type::string:
      {
        size_t Size = 0;
        is.read(reinterpret_cast<char *>(&Size), sizeof(size_t));

        char str[Size + 1];
        str[Size] = 0;
        is.read(str, Size * sizeof(char));
        *static_cast< std::string * >(mpValue) = str;
      }
      break;
  }
}

CValueInterface & CValueInterface::operator=(const CValueInterface & rhs)
{
  if (this != &rhs)
    {
      switch (mType)
      {
        case Type::boolean:
          *static_cast< bool * >(mpValue) = *static_cast< const bool * >(rhs.mpValue);
          break;

        case Type::number:
          *static_cast< double * >(mpValue) = *static_cast< const double * >(rhs.mpValue);
          break;

        case Type::traitData:
          *static_cast< CTraitData::base * >(mpValue) = *static_cast< const CTraitData::base * >(rhs.mpValue);
          break;

        case Type::traitValue:
          *static_cast< CTraitData::value * >(mpValue) = *static_cast< const CTraitData::value * >(rhs.mpValue);
          break;

        case Type::string:
          *static_cast< std::string * >(mpValue) = *static_cast< const std::string * >(rhs.mpValue);
          break;

        case Type::healthState:
        case Type::id:
          *static_cast< size_t * >(mpValue) = *static_cast< const size_t * >(rhs.mpValue);
          break;
      }
    }

  return *this;
}

// static
void CValueInterface::equal(double & lhs, const double & rhs)
{
  lhs = rhs;
}

// static
void CValueInterface::plus(double & lhs, const double & rhs)
{
  lhs += rhs;
}

// static
void CValueInterface::minus(double & lhs, const double & rhs)
{
  lhs -= rhs;
}

// static
void CValueInterface::multiply(double & lhs, const double & rhs)
{
  lhs *= rhs;
}

// static
void CValueInterface::divide(double & lhs, const double & rhs)
{
  lhs /= rhs;
}


bool operator<(const CValueInterface & lhs, const CValueInterface & rhs)
{
  if (lhs.mType != rhs.mType)
    return lhs.mType < rhs.mType;

  switch (lhs.mType)
  {
    case CValueInterface::Type::boolean:
      return * static_cast< const bool * >(lhs.mpValue) < * static_cast< const bool * >(rhs.mpValue);
      break;

    case CValueInterface::Type::number:
      return * static_cast< const double * >(lhs.mpValue) < * static_cast< const double * >(rhs.mpValue);
      break;

    case CValueInterface::Type::healthState:
    case CValueInterface::Type::id:
      return * static_cast< const size_t * >(lhs.mpValue) < * static_cast< const size_t * >(rhs.mpValue);
      break;

    case CValueInterface::Type::traitData:
      return * static_cast< const CTraitData::base * >(lhs.mpValue) < * static_cast< const CTraitData::base * >(rhs.mpValue);
      break;

    case CValueInterface::Type::traitValue:
      return * static_cast< const CTraitData::value * >(lhs.mpValue) < * static_cast< const CTraitData::value * >(rhs.mpValue);
      break;

    case CValueInterface::Type::string:
      return * static_cast< std::string * >(lhs.mpValue) < * static_cast< const std::string * >(rhs.mpValue);
      break;
  }

  return lhs.mpValue < rhs.mpValue;
}

bool operator<=(const CValueInterface & lhs, const CValueInterface & rhs)
{
  if (lhs.mType != rhs.mType)
    return lhs.mType <= rhs.mType;

  switch (lhs.mType)
  {
    case CValueInterface::Type::boolean:
      return * static_cast< const bool * >(lhs.mpValue) <= * static_cast< const bool * >(rhs.mpValue);
      break;

    case CValueInterface::Type::number:
      return * static_cast< const double * >(lhs.mpValue) <= * static_cast< const double * >(rhs.mpValue);
      break;

    case CValueInterface::Type::healthState:
    case CValueInterface::Type::id:
      return * static_cast< const size_t * >(lhs.mpValue) <= * static_cast< const size_t * >(rhs.mpValue);
      break;

    case CValueInterface::Type::traitData:
      return * static_cast< const CTraitData::base * >(lhs.mpValue) <= * static_cast< const CTraitData::base * >(rhs.mpValue);
      break;

    case CValueInterface::Type::traitValue:
      return * static_cast< const CTraitData::value * >(lhs.mpValue) <= * static_cast< const CTraitData::value * >(rhs.mpValue);
      break;

    case CValueInterface::Type::string:
      return * static_cast< std::string * >(lhs.mpValue) <= * static_cast< const std::string * >(rhs.mpValue);
      break;
  }

  return lhs.mpValue <= rhs.mpValue;
}

// static
bool operator>(const CValueInterface & lhs, const CValueInterface & rhs)
{
  return operator<(rhs, lhs);
}

// static
bool operator>=(const CValueInterface & lhs, const CValueInterface & rhs)
{
  return operator<=(rhs, lhs);
}

bool operator==(const CValueInterface & lhs, const CValueInterface & rhs)
{
  if (lhs.mType == CValueInterface::Type::traitData && rhs.mType == CValueInterface::Type::traitValue)
    {
      return CTraitData::hasValue(* static_cast< const CTraitData::base * >(lhs.mpValue),
                                   * static_cast< const CTraitData::value * >(rhs.mpValue));
    }

  if (lhs.mType == CValueInterface::Type::traitValue && rhs.mType == CValueInterface::Type::traitData)
    {
      return CTraitData::hasValue(* static_cast< const CTraitData::base * >(rhs.mpValue),
                                   * static_cast< const CTraitData::value * >(lhs.mpValue));
    }

  if (lhs.mType != rhs.mType)
    return false;

  switch (lhs.mType)
  {
    case CValueInterface::Type::boolean:
      return * static_cast< const bool * >(lhs.mpValue) == * static_cast< const bool * >(rhs.mpValue);
      break;

    case CValueInterface::Type::number:
      return * static_cast< const double * >(lhs.mpValue) == * static_cast< const double * >(rhs.mpValue);
      break;

    case CValueInterface::Type::healthState:
    case CValueInterface::Type::id:
      return * static_cast< const size_t * >(lhs.mpValue) == * static_cast< const size_t * >(rhs.mpValue);
      break;

    case CValueInterface::Type::traitData:
      return * static_cast< const CTraitData::base * >(lhs.mpValue) == * static_cast< const CTraitData::base * >(rhs.mpValue);
      break;

    case CValueInterface::Type::traitValue:
      return * static_cast< const CTraitData::value * >(lhs.mpValue) == * static_cast< const CTraitData::value * >(rhs.mpValue);
      break;

    case CValueInterface::Type::string:
      return * static_cast< std::string * >(lhs.mpValue) == * static_cast< const std::string * >(rhs.mpValue);
      break;
  }

  return lhs.mpValue == rhs.mpValue;
}

bool operator!=(const CValueInterface & lhs, const CValueInterface & rhs)
{
  return !operator==(lhs, rhs);
}

/*
const CValueInterface & CValueInterface::operator=(const CValueInterface & rhs)
{
  if (mType == Type::traitData && rhs.mType == Type::traitValue)
    {
      CTraitData::setValue(* static_cast< CTraitData::base * >(mpValue),
                           * static_cast< const CTraitData::value * >(rhs.mpValue));

      return *this;
    }

  if (mType != rhs.mType)
    {
      FatalError(CCommunicate::ErrorCode::TypeMissmatch, "Type mismatch");
    }

  assignValue(rhs.mpValue);
  return *this;
}
*/



