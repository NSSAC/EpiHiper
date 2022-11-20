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
 * Feature.h
 *
 *  Created on: Jun 21, 2019
 *      Author: shoops
 */

#ifndef SRC_TRAITS_CFEATURE_H_
#define SRC_TRAITS_CFEATURE_H_

#include <string>
#include <vector>
#include <map>

#include "traits/CEnum.h"

class CFeature: public CAnnotation
{
public:
  /**
   * Default constructor
   */
  CFeature(const std::string & id = "");

  /**
   * Copy construnctor
   * @param const Feature & src
   */
  CFeature(const CFeature & src);

  /**
   * Destructor
   */
  virtual ~CFeature();

	CFeature & operator=(const CFeature & rhs);

  virtual void fromJSON(const json_t * json) override;

  void augment(const json_t * json);

  const std::string & getId() const;

  const bool & isValid() const;

  size_t bitsRequired() const;

  void setMask(const CTraitData::base & mask);

  const CTraitData::base & getMask() const;

  const CEnum * operator[](const size_t & index) const;
  const CEnum * operator[](const std::string & ) const;

  const CEnum * getDefault() const;

private:
  const CEnum * addEnum(const CEnum & enumeration);

  void updateEnumMap();

  std::string mId;
  std::string mDefaultId;
  CTraitData::base mMask;
  std::vector< CEnum > mEnums;
  std::map< std::string, CEnum * > mEnumMap;
  bool mValid;
};

#endif /* SRC_TRAITS_CFEATURE_H_ */
