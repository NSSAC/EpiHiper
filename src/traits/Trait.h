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
 * Traits.h
 *
 *  Created on: Jun 21, 2019
 *      Author: shoops
 */

#ifndef SRC_TRAITS_TRAIT_H_
#define SRC_TRAITS_TRAIT_H_

#include <vector>
#include <map>
#include <utility>

#include "traits/Feature.h"

struct json_t;

class Trait: public Annotation
{
public:

  static std::map< std::string, Trait > INSTANCES;
  static Trait const * ActivityTrait;
  static Trait const * EdgeTrait;
  static Trait const * NodeTrait;


  static void init();

  static void load(const std::string & jsonFile);

	/**
	 * Default constructor
	 */
	Trait();

	/**
	 * Copy construnctor
	 * @param const Trait & src
	 */
	Trait(const Trait & src);

	/**
	 * Destructor
	 */
	virtual ~Trait();

	void fromJSON(const json_t * json);

	const std::string & getId() const;

	const bool & isValid() const;

	size_t size() const;

  const Feature & operator[](const size_t & index) const;

  const Feature & operator[](const std::string & id) const;

	TraitData::base getDefault() const;

  bool fromString(const std::string & str, TraitData::base & data) const;

  std::string toString(TraitData::base & data) const;

private:
	std::string mId;
	size_t mBytes;
	std::vector< Feature > mFeatures;
	std::map< std::string, Feature * > mFeatureMap;
	bool mValid;
};

#endif /* SRC_TRAITS_TRAIT_H_ */
