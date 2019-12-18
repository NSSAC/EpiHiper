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

  virtual void fromJSON(const json_t * json);

  void augment(const json_t * json);

  const std::string & getId() const;

  const bool & isValid() const;

  size_t size() const;

  size_t bitsRequired() const;

  void setMask(const CTraitData::base & mask);

  const CTraitData::base & getMask() const;

  const CEnum * operator[](const size_t & index) const;
  const CEnum * operator[](const std::string & ) const;

  const CEnum * getDefault() const;

  const CEnum * addEnum(const CEnum & enumeration);

private:
  void updateEnumMap();

  std::string mId;
  std::string mDefaultId;
  CTraitData::base mMask;
  std::vector< CEnum > mEnums;
  std::map< std::string, CEnum * > mEnumMap;
  bool mValid;
};

#endif /* SRC_TRAITS_CFEATURE_H_ */
