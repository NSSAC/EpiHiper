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

/*
 * MetaData.cpp
 *
 *  Created on: Jul 11, 2019
 *      Author: shoops
 */

#include <limits>
#include <iostream>
#include <jansson.h>

#include "utilities/CMetadata.h"

#define CMETADATA_BUFFER_SIZE 1024

CMetadata::CMetadata()
  : mpJson(NULL)
{}

CMetadata::CMetadata(json_t * pObject)
  : mpJson(json_incref(pObject))
{}

CMetadata::CMetadata(const std::string & key, const std::string & value)
  : mpJson(json_object())
{
  set(key, value);
}

CMetadata::CMetadata(const std::string & key, const bool & value)
  : mpJson(json_object())
{
  set(key, value);
}

CMetadata::CMetadata(const std::string & key, const double & value)
  : mpJson(json_object())
{
  set(key, value);
}

CMetadata::CMetadata(const std::string & key, const int & value)
  : mpJson(json_object())
{
  set(key, value);
}

CMetadata::CMetadata(const std::string & key, const CMetadata & value)
  : mpJson(json_object())
{
  set(key, value);
}

CMetadata::CMetadata(const CMetadata & src)
  : mpJson(json_incref(src.mpJson))
{}

// virtual
CMetadata::~CMetadata()
{
  json_decref(mpJson);
}

void CMetadata::set(const std::string & key, json_t * pValue)
{
  if (!mpJson)
    {
      mpJson = json_object();
    }
  else
    {
      json_t * pJson = mpJson;
      mpJson = json_deep_copy(pJson);
      json_decref(pJson);
    }

  json_object_set(mpJson, key.c_str(), pValue);
  json_decref(pValue);
}

void CMetadata::set(const std::string & key, const std::string & value)
{
  set(key, json_string(value.c_str()));
}

void CMetadata::set(const std::string & key, const bool & value)
{
  set(key, json_boolean(value));
}

void CMetadata::set(const std::string & key, const double & value)
{
  set(key, json_real(value));
}

void CMetadata::set(const std::string & key, const int & value)
{
  set(key, json_integer(value));
}

void CMetadata::set(const std::string & key, const CMetadata & value)
{
  json_object_set(mpJson, key.c_str(), value.mpJson);
}

bool CMetadata::get(const std::string & key, json_t *& pValue) const
{
  pValue = NULL;

  if (mpJson != NULL)
    {
      pValue = json_object_get(mpJson, key.c_str());
    }

  return (pValue != NULL);
}

bool CMetadata::contains(const std::string & key) const
{
  json_t * pValue;

  return get(key, pValue);
}

CMetadata::Type CMetadata::getType(const std::string & key) const
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

std::string CMetadata::getString(const std::string & key) const
{
  json_t * pValue;

  if (get(key, pValue) && json_is_string(pValue))
    {
      return json_string_value(pValue);
    }

  return "";
}

bool CMetadata::getBool(const std::string & key) const
{
  json_t * pValue;

  if (get(key, pValue) && json_is_boolean(pValue))
    {
      return json_boolean_value(pValue);
    }

  return false;
}

double CMetadata::getDouble(const std::string & key) const
{
  json_t * pValue;

  if (get(key, pValue) && json_is_real(pValue))
    {
      return json_real_value(pValue);
    }

  return std::numeric_limits< double >::quiet_NaN();
}

int CMetadata::getInt(const std::string & key) const
{
  json_t * pValue;

  if (get(key, pValue) && json_is_integer(pValue))
    {
      return json_integer_value(pValue);
    }

  return 0;
}

CMetadata CMetadata::getObject(const std::string & key) const
{
  json_t * pValue;

  if (get(key, pValue) && json_is_object(pValue))
    {
      return CMetadata(pValue);
    }

  return CMetadata();
}

void CMetadata::toBinary(std::ostream & os) const
{
  static size_t BufferSize = CMETADATA_BUFFER_SIZE;
  static char * pBuffer = new char[CMETADATA_BUFFER_SIZE];

  size_t size = (mpJson != NULL) ? json_dumpb(mpJson, pBuffer, BufferSize, JSON_COMPACT | JSON_INDENT(0)) : 0;

  if (pBuffer == NULL || size > BufferSize)
    {
      if (pBuffer != NULL)
        delete[] pBuffer;

      BufferSize = std::max(BufferSize, size);

      pBuffer = new char[BufferSize];

      json_dumpb(mpJson, pBuffer,BufferSize, JSON_COMPACT | JSON_INDENT(0));
    }

  os.write(reinterpret_cast< const char * >(&size), sizeof(size_t));
  os.write(pBuffer, size);
}

bool CMetadata::fromBinary(std::istream & is)
{
  static size_t BufferSize = 1024;
  static char * pBuffer = NULL;

  size_t size;
  is.read(reinterpret_cast< char * >(&size), sizeof(size_t));

  if (pBuffer == NULL || size > BufferSize)
    {
      if (pBuffer != NULL)
        delete[] pBuffer;

      BufferSize = std::max(BufferSize, size);

      pBuffer = new char[BufferSize];
    }

  is.read(pBuffer, size);

  json_decref(mpJson);
  json_error_t error;

  mpJson = json_loadb(pBuffer, size, 0, &error);

  return (mpJson != NULL);
}
