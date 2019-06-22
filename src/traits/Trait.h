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

#include <map>
#include <utility>

#include "Feature.h"

struct json_t;

class Trait: public Annotation
{
public:

	static std::map< std::string, Trait > load(const std::string & jsonFile);

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

	const Feature & getFeature(const std::string & id) const;

private:
	std::string mId;
	size_t mBytes;
	std::map< std::string, Feature > mFeatureMap;
	bool mValid;
};

#endif /* SRC_TRAITS_TRAIT_H_ */
