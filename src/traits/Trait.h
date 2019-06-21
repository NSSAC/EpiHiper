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

class Trait
{
public:

	class Data
	{
	public:
		Data() = delete;

		Data(const Trait & trait);

		~Data();

	private:
		char * mpBuffer;
		size_t mBytes;
	};

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

	size_t size() const;

private:
	std::string mName;
	size_t mBytes;
	std::map< std::string, std::pair< Data, std::map< std::string, Data > > > mFeatureMask;
};

#endif /* SRC_TRAITS_TRAIT_H_ */
