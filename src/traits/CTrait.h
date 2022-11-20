// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
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
 * Traits.h
 *
 *  Created on: Jun 21, 2019
 *      Author: shoops
 */

#ifndef SRC_TRAITS_CTRAIT_H_
#define SRC_TRAITS_CTRAIT_H_

#include <vector>
#include <map>
#include <utility>

#include "traits/CFeature.h"

struct json_t;

class CTrait : public CAnnotation
{
private:
  static std::map< std::string, CTrait > INSTANCES;
public:
  static CTrait const * ActivityTrait;
  static CTrait const * EdgeTrait;
  static CTrait const * NodeTrait;

  static void init();

  static const CTrait * find(const std::string & id);

  static void load(const std::string & jsonFile);

  static void load(const json_t * json);

  static void loadSingle(const json_t * json);

  /**
   * Default constructor
   */
  CTrait();

  /**
   * Copy construnctor
   * @param const Trait & src
   */
  CTrait(const CTrait & src);

  /**
   * Destructor
   */
  virtual ~CTrait();

  CTrait & operator=(const CTrait & rhs);

  virtual void fromJSON(const json_t * json) override;

  const std::string & getId() const;

  const bool & isValid() const;

  size_t size() const;

  const CFeature * operator[](const size_t & index) const;

  const CFeature * operator[](const std::string & id) const;

  const CFeature * operator[](const json_t * json) const;

  CTraitData::base getDefault() const;

  bool fromString(const char * str, CTraitData::base & data) const;

  std::string toString(CTraitData::base & data) const;

  std::string toString(CTraitData::value & value) const;

  void remap();

private:
  const CFeature * addFeature(const CFeature & feature);

  void updateFeatureMap();

  std::string mId;
  size_t mBytes;
  std::vector< CFeature > mFeatures;
  std::map< std::string, CFeature * > mFeatureMap;
  mutable std::map< CTraitData::base, std::string > mTextEncoding;
  mutable std::map< std::string, CTraitData::base > mTextDecoding;
  mutable std::map< CTraitData::value, std::string > mValueEncoding;
  bool mValid;
};

#endif /* SRC_TRAITS_CTRAIT_H_ */
