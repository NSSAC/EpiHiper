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

#ifndef SRC_TRAITS_FEATURE_H_
#define SRC_TRAITS_FEATURE_H_

#include <string>
#include <map>

#include "Enum.h"

class Feature: public Annotation
{
public:
  /**
   * Default constructor
   */
  Feature();

  /**
   * Copy construnctor
   * @param const Feature & src
   */
  Feature(const Feature & src);

  /**
   * Destructor
   */
  virtual ~Feature();

  void fromJSON(const json_t * json);

  const std::string & getId() const;

  const bool & isValid() const;

  size_t size() const;

  size_t bitsRequired() const;

  void setMask(const TraitData & mask);

  const TraitData & getMask() const;

  const Enum & getEnum(const std::string & ) const;

private:
  std::string mId;
  std::string mDefault;
  TraitData mMask;
  std::map< std::string, Enum > mEnumMap;
  bool mValid;
};

#endif /* SRC_TRAITS_FEATURE_H_ */