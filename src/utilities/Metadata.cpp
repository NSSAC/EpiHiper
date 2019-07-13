/*
 * MetaData.cpp
 *
 *  Created on: Jul 11, 2019
 *      Author: shoops
 */

#include <limits>
#include <iostream>

#include <jansson.h>

#include "Metadata.h"

Metadata::Metadata()
  : mpJson(NULL)
{}

Metadata::Metadata(json_t * pObject)
  : mpJson(json_incref(pObject))
{}

Metadata::Metadata(const std::string & key, const std::string & value)
  : mpJson(json_object())
{
  set(key, value);
}

Metadata::Metadata(const std::string & key, const bool & value)
  : mpJson(json_object())
{
  set(key, value);
}

Metadata::Metadata(const std::string & key, const double & value)
  : mpJson(json_object())
{
  set(key, value);
}

Metadata::Metadata(const std::string & key, const int & value)
  : mpJson(json_object())
{
  set(key, value);
}

Metadata::Metadata(const std::string & key, const Metadata & value)
  : mpJson(json_object())
{
  set(key, value);
}

Metadata::Metadata(const Metadata & src)
  : mpJson(json_incref(src.mpJson))
{}

// virtual
Metadata::~Metadata()
{
  json_decref(mpJson);
}

void Metadata::set(const std::string & key, json_t * pValue)
{
  json_object_set(mpJson, key.c_str(), pValue);
  json_decref(pValue);
}

void Metadata::set(const std::string & key, const std::string & value)
{
  set(key, json_string(value.c_str()));
}

void Metadata::set(const std::string & key, const bool & value)
{
  set(key, json_boolean(value));
}

void Metadata::set(const std::string & key, const double & value)
{
  set(key, json_real(value));
}

void Metadata::set(const std::string & key, const int & value)
{
  set(key, json_integer(value));
}

void Metadata::set(const std::string & key, const Metadata & value)
{
  json_object_set(mpJson, key.c_str(), value.mpJson);
}

bool Metadata::get(const std::string & key, json_t *& pValue) const
{
  pValue = NULL;

  if (mpJson != NULL)
    {
      pValue = json_object_get(mpJson, key.c_str());
    }

  return (pValue != NULL);
}

bool Metadata::contains(const std::string & key) const
{
  json_t * pValue;

  return get(key, pValue);
}

Metadata::Type Metadata::getType(const std::string & key) const
{
  json_t * pValue;

  if (get(key, pValue))
    {
      switch (json_typeof(pValue))
      {
        case JSON_OBJECT:
          return Type::Object;
          break;

        case JSON_ARRAY:
          return Type::Array;
          break;

        case JSON_STRING:
          return Type::String;
          break;

        case JSON_INTEGER:
          return Type::Integer;
          break;

        case JSON_REAL:
          return Type::Double;
          break;

        case JSON_TRUE:
          return Type::Boolean;
          break;

        case JSON_FALSE:
          return Type::Boolean;
          break;

        case JSON_NULL:
          return Type::Null;
          break;
      }
    }

  return Type::NotFound;
}

std::string Metadata::getString(const std::string & key) const
{
  json_t * pValue;

  if (get(key, pValue) && json_is_string(pValue))
    {
      return json_string_value(pValue);
    }

  return "";
}

bool Metadata::getBool(const std::string & key) const
{
  json_t * pValue;

  if (get(key, pValue) && json_is_boolean(pValue))
    {
      return json_boolean_value(pValue);
    }

  return false;
}

double Metadata::getDouble(const std::string & key) const
{
  json_t * pValue;

  if (get(key, pValue) && json_is_real(pValue))
    {
      return json_real_value(pValue);
    }

  return std::numeric_limits< double >::quiet_NaN();
}

int Metadata::getInt(const std::string & key) const
{
  json_t * pValue;

  if (get(key, pValue) && json_is_integer(pValue))
    {
      return json_integer_value(pValue);
    }

  return 0;
}

Metadata Metadata::getObject(const std::string & key) const
{
  json_t * pValue;

  if (get(key, pValue) && json_is_object(pValue))
    {
      return Metadata(pValue);
    }

  return Metadata();
}

void Metadata::toBinary(std::ostream & os) const
{
  static size_t BufferSize = 1024;
  static char * pBuffer = NULL;

  size_t size = (mpJson != NULL) ? json_dumpb(mpJson, pBuffer, 0, 0) : 0;

  if (pBuffer == NULL || size > BufferSize)
    {
      if (pBuffer != NULL)
        delete [] pBuffer;

      BufferSize = std::max(BufferSize, size);

      pBuffer = new char[BufferSize];

      json_dumpb(mpJson, pBuffer, size, 0);
    }

  os.write(reinterpret_cast<const char *>(&size), sizeof(size_t));
  os.write(pBuffer, size);
}

bool Metadata::fromBinary(std::istream & is)
{
  static size_t BufferSize = 1024;
  static char * pBuffer = NULL;

  size_t size;
  is.read(reinterpret_cast<char *>(&size), sizeof(size_t));

  if (pBuffer == NULL || size > BufferSize)
    {
      if (pBuffer != NULL)
        delete [] pBuffer;

      BufferSize = std::max(BufferSize, size);

      pBuffer = new char[BufferSize];
    }

  is.read(pBuffer, size);

  json_decref(mpJson);
  json_error_t error;

  mpJson = json_loadb(pBuffer, size, 0, &error);

  return (mpJson != NULL);
}

