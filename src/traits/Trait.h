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

struct json_t;

class Feature;
class Trait
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

	const std::string & getName() const;

	const bool & isValid() const;

	size_t size() const;

	const Feature & getFeature(const std::string & name) const;

private:
	std::string mName;
	size_t mBytes;
	std::map< std::string, Feature > mFeatureMap;
	bool mValid;
};

#endif /* SRC_TRAITS_TRAIT_H_ */
