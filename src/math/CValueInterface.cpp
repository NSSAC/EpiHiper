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

#include "math/CValueInterface.h"
#include "utilities/CCommunicate.h"

// static 
const CEnumAnnotation< std::string, CValueInterface::Type > CValueInterface::TypeName({
  "Boolean",
  "number",
  "integer",
  "trait data",
  "trait value",
  "string",
  "id"
});

// static 
const CEnumAnnotation< std::string, CValueInterface::Operator > CValueInterface::OperatorName({
  "=",
  "+=",
  "-=",
  "*=",
  "/="
});

// static 
const CEnumAnnotation< CValueInterface::pOperator, CValueInterface::Operator > CValueInterface::OperatorFunction({
  &CValueInterface::equal,
  &CValueInterface::plus,
  &CValueInterface::minus,
  &CValueInterface::multiply,
  &CValueInterface::divide
});

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

const double & CValueInterface::toNumber() const
{
  return * static_cast< double * >(mpValue);
}

const int & CValueInterface::toInteger() const
{
  return * static_cast< int * >(mpValue);
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

    case Type::integer:
      os.write(reinterpret_cast<const char *>(mpValue), sizeof(int));
      break;

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

    case Type::__SIZE:
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

    case Type::integer:
      is.read(reinterpret_cast<char *>(&mpValue), sizeof(int));
      break;

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

    case Type::__SIZE:
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

        case CValueInterface::Type::integer:
          *static_cast< int * >(mpValue) = *static_cast< const int * >(rhs.mpValue);
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

        case Type::id:
          *static_cast< size_t * >(mpValue) = *static_cast< const size_t * >(rhs.mpValue);
          break;

        case Type::__SIZE:
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

// static 
bool CValueInterface::compatible(const CValueInterface::Type & lhs, const CValueInterface::Type & rhs)
{
  bool compatible = false;

  switch (lhs)
    {
      case Type::boolean:
      case Type::string:
      case Type::id:
        compatible = (lhs == rhs);
        break;

      case Type::integer:
      case Type::number:
        compatible = (rhs == Type::integer || rhs == Type::number);
        break;

      case Type::traitData:
        compatible = (rhs == Type::traitValue);
        break;

      case Type::traitValue:
        compatible = (rhs == Type::traitData);
        break;

      case CValueInterface::Type::__SIZE:
        break;
    }

  return compatible;
}

// static 
std::string CValueInterface::operatorToString(CValueInterface::pOperator pOperator)
{
  if (pOperator == CValueInterface::equal)
    return "=";

  if (pOperator == CValueInterface::plus)
    return "+=";

  if (pOperator == CValueInterface::minus)
    return "-=";

  if (pOperator == CValueInterface::multiply)
    return "*=";

  if (pOperator == CValueInterface::divide)
    return "/=";

  return "N/A";
}


bool operator<(const CValueInterface & lhs, const CValueInterface & rhs)
{
  if (lhs.mType != rhs.mType)
    {
        if (lhs.mType == CValueInterface::Type::__SIZE
            || rhs.mType == CValueInterface::Type::__SIZE)
        return false;

        return lhs.mType < rhs.mType;
    }

  switch (lhs.mType)
  {
    case CValueInterface::Type::boolean:
      return * static_cast< const bool * >(lhs.mpValue) < * static_cast< const bool * >(rhs.mpValue);
      break;

    case CValueInterface::Type::number:
      return * static_cast< const double * >(lhs.mpValue) < * static_cast< const double * >(rhs.mpValue);
      break;

    case CValueInterface::Type::integer:
      return * static_cast< const int * >(lhs.mpValue) < * static_cast< const int * >(rhs.mpValue);
      break;

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

    case CValueInterface::Type::__SIZE:
      break;
  }

  return false;
}

bool operator<=(const CValueInterface & lhs, const CValueInterface & rhs)
{
  if (lhs.mType != rhs.mType)
    {
        if (lhs.mType == CValueInterface::Type::__SIZE
            || rhs.mType == CValueInterface::Type::__SIZE)
        return false;

      return lhs.mType <= rhs.mType;
    }

  switch (lhs.mType)
  {
    case CValueInterface::Type::boolean:
      return * static_cast< const bool * >(lhs.mpValue) <= * static_cast< const bool * >(rhs.mpValue);
      break;

    case CValueInterface::Type::number:
      return * static_cast< const double * >(lhs.mpValue) <= * static_cast< const double * >(rhs.mpValue);
      break;

    case CValueInterface::Type::integer:
      return * static_cast< const int * >(lhs.mpValue) <= * static_cast< const int * >(rhs.mpValue);
      break;

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

    case CValueInterface::Type::__SIZE:
      break;
  }

  return false;
}

// static
bool operator>(const CValueInterface & lhs, const CValueInterface & rhs)
{
  if (lhs.mType != rhs.mType)
    {
        if (lhs.mType == CValueInterface::Type::__SIZE
            || rhs.mType == CValueInterface::Type::__SIZE)
        return false;

      return lhs.mType > rhs.mType;
    }

  switch (lhs.mType)
  {
    case CValueInterface::Type::boolean:
      return * static_cast< const bool * >(lhs.mpValue) > * static_cast< const bool * >(rhs.mpValue);
      break;

    case CValueInterface::Type::number:
      return * static_cast< const double * >(lhs.mpValue) > * static_cast< const double * >(rhs.mpValue);
      break;

    case CValueInterface::Type::integer:
      return * static_cast< const int * >(lhs.mpValue) > * static_cast< const int * >(rhs.mpValue);
      break;

    case CValueInterface::Type::id:
      return * static_cast< const size_t * >(lhs.mpValue) > * static_cast< const size_t * >(rhs.mpValue);
      break;

    case CValueInterface::Type::traitData:
      return * static_cast< const CTraitData::base * >(lhs.mpValue) > * static_cast< const CTraitData::base * >(rhs.mpValue);
      break;

    case CValueInterface::Type::traitValue:
      return * static_cast< const CTraitData::value * >(lhs.mpValue) > * static_cast< const CTraitData::value * >(rhs.mpValue);
      break;

    case CValueInterface::Type::string:
      return * static_cast< std::string * >(lhs.mpValue) > * static_cast< const std::string * >(rhs.mpValue);
      break;

    case CValueInterface::Type::__SIZE:
      break;
  }

  return false;
}

// static
bool operator>=(const CValueInterface & lhs, const CValueInterface & rhs)
{
   if (lhs.mType != rhs.mType)
    {
        if (lhs.mType == CValueInterface::Type::__SIZE
            || rhs.mType == CValueInterface::Type::__SIZE)
        return false;

      return lhs.mType >= rhs.mType;
    }

  switch (lhs.mType)
  {
    case CValueInterface::Type::boolean:
      return * static_cast< const bool * >(lhs.mpValue) >= * static_cast< const bool * >(rhs.mpValue);
      break;

    case CValueInterface::Type::number:
      return * static_cast< const double * >(lhs.mpValue) >= * static_cast< const double * >(rhs.mpValue);
      break;

    case CValueInterface::Type::integer:
      return * static_cast< const int * >(lhs.mpValue) >= * static_cast< const int * >(rhs.mpValue);
      break;

    case CValueInterface::Type::id:
      return * static_cast< const size_t * >(lhs.mpValue) >= * static_cast< const size_t * >(rhs.mpValue);
      break;

    case CValueInterface::Type::traitData:
      return * static_cast< const CTraitData::base * >(lhs.mpValue) >= * static_cast< const CTraitData::base * >(rhs.mpValue);
      break;

    case CValueInterface::Type::traitValue:
      return * static_cast< const CTraitData::value * >(lhs.mpValue) >= * static_cast< const CTraitData::value * >(rhs.mpValue);
      break;

    case CValueInterface::Type::string:
      return * static_cast< std::string * >(lhs.mpValue) >= * static_cast< const std::string * >(rhs.mpValue);
      break;

    case CValueInterface::Type::__SIZE:
      break;
  }

  return false;
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

    case CValueInterface::Type::integer:
      return * static_cast< const int * >(lhs.mpValue) == * static_cast< const int * >(rhs.mpValue);
      break;

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

    case CValueInterface::Type::__SIZE:
      break;
  }

  return false;
}

bool operator!=(const CValueInterface & lhs, const CValueInterface & rhs)
{
  if (lhs.mType == CValueInterface::Type::traitData && rhs.mType == CValueInterface::Type::traitValue)
    {
      return !CTraitData::hasValue(* static_cast< const CTraitData::base * >(lhs.mpValue),
                                   * static_cast< const CTraitData::value * >(rhs.mpValue));
    }

  if (lhs.mType == CValueInterface::Type::traitValue && rhs.mType == CValueInterface::Type::traitData)
    {
      return !CTraitData::hasValue(* static_cast< const CTraitData::base * >(rhs.mpValue),
                                   * static_cast< const CTraitData::value * >(lhs.mpValue));
    }

  if (lhs.mType != rhs.mType)
    return true;

  switch (lhs.mType)
  {
    case CValueInterface::Type::boolean:
      return * static_cast< const bool * >(lhs.mpValue) != * static_cast< const bool * >(rhs.mpValue);
      break;

    case CValueInterface::Type::number:
      return * static_cast< const double * >(lhs.mpValue) != * static_cast< const double * >(rhs.mpValue);
      break;

    case CValueInterface::Type::integer:
      return * static_cast< const int * >(lhs.mpValue) != * static_cast< const int * >(rhs.mpValue);
      break;

    case CValueInterface::Type::id:
      return * static_cast< const size_t * >(lhs.mpValue) != * static_cast< const size_t * >(rhs.mpValue);
      break;

    case CValueInterface::Type::traitData:
      return * static_cast< const CTraitData::base * >(lhs.mpValue) != * static_cast< const CTraitData::base * >(rhs.mpValue);
      break;

    case CValueInterface::Type::traitValue:
      return * static_cast< const CTraitData::value * >(lhs.mpValue) != * static_cast< const CTraitData::value * >(rhs.mpValue);
      break;

    case CValueInterface::Type::string:
      return * static_cast< std::string * >(lhs.mpValue) != * static_cast< const std::string * >(rhs.mpValue);
      break;

    case CValueInterface::Type::__SIZE:
      break;
  }

  return true;
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

std::ostream & operator << (std::ostream & os, const CValueInterface & p)
{
  switch (p.mType)
  {
    case CValueInterface::Type::boolean:
      os << * static_cast< const bool * >(p.mpValue);
      break;

    case CValueInterface::Type::number:
      os << * static_cast< const double * >(p.mpValue);
      break;

    case CValueInterface::Type::integer:
      os << * static_cast< const int * >(p.mpValue);
      break;

    case CValueInterface::Type::id:
      os << * static_cast< const size_t * >(p.mpValue);
      break;

    case CValueInterface::Type::traitData:
      os << * static_cast< const CTraitData::base * >(p.mpValue);
      break;

    case CValueInterface::Type::traitValue:
      os << static_cast< const CTraitData::value * >(p.mpValue)->first << ":" << static_cast< const CTraitData::value * >(p.mpValue)->second;
      break;

    case CValueInterface::Type::string:
      os << * static_cast< const std::string * >(p.mpValue);
      break;

    case CValueInterface::Type::__SIZE:
      break;      
  }

  return os;
}

