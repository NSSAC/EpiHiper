// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
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

#ifndef SRC_TRAITS_CTRAIT_H_
#define SRC_TRAITS_CTRAIT_H_

#include <vector>
#include <map>
#include <utility>

#include "traits/CFeature.h"

struct json_t;

class CTrait: public CAnnotation
{
public:

  static std::map< std::string, CTrait > INSTANCES;
  static CTrait const * ActivityTrait;
  static CTrait const * EdgeTrait;
  static CTrait const * NodeTrait;


  static void init();

  static void load(const std::string & jsonFile);

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
	bool mValid;
};

#endif /* SRC_TRAITS_CTRAIT_H_ */
