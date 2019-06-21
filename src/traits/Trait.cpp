/*
 * Traits.cpp
 *
 *  Created on: Jun 21, 2019
 *      Author: shoops
 */

#include <fstream>
#include <iostream>
#include <jansson.h>

#include "Trait.h"

// static
std::map< std::string, Trait > Trait::load(const std::string & jsonFile)
{
  std::map< std::string, Trait > Traits;

  if (jsonFile.empty())
    {
      std::cerr << "Traits file is not specified" << std::endl;
      return Traits;
    }

  std::ifstream is(jsonFile.c_str());

  if (is.fail())
    {
      std::cerr << "Traits file: '" << jsonFile << "' cannot be opened." << std::endl;
      return Traits;
    }

  // get length of file:
  is.seekg (0, is.end);
  std::istream::pos_type length = is.tellg();
  is.seekg (0, is.beg);

  char * buffer = new char [length];
  is.read(buffer, length);

  json_t *pRoot;
  json_error_t error;

  pRoot = json_loads(buffer, 0, &error);
  delete [] buffer;

  if (pRoot == NULL)
    {
      std::cerr << "Traits file: '" << jsonFile << "' JSON error on line " << error.line << ": " << error.text << std::endl;
      delete [] buffer;

      return Traits;
    }

  // Get the key "traits"
  json_t * pTraits = json_object_get(pRoot, "traits");

  // Iterate of the array elements
  for (size_t i = 0, imax = json_array_size(pTraits); i < imax; ++i)
    {
      Trait Trait;
      Trait.fromJSON(json_array_get(pTraits, i));

      Traits[Trait.getName()] = Trait;
    }

  json_decref(pRoot);

  return Traits;
}


Trait::Data::Data(const Trait & trait)
  : mpBuffer(NULL)
  , mBytes(trait.size())
{
  mpBuffer = new char[mBytes];
}

Trait::Data::~Data()
{
  delete [] mpBuffer;
}

Trait::Trait()
  : mName()
  , mBytes(0)
  , mFeatureMask()
{}

Trait::~Trait() {}

Trait::Trait(const Trait & src)
  : mName(src.mName)
  , mBytes(src.mBytes)
  , mFeatureMask(src.mFeatureMask)
{}

void Trait::fromJSON(const json_t * json)
{

}

const std::string & Trait::getName() const
{
  return mName;
}


size_t Trait::size() const
{
  return mBytes;
}


