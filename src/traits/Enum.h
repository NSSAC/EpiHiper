/*
 * Enum.h
 *
 *  Created on: Jun 22, 2019
 *      Author: shoops
 */

#ifndef SRC_TRAITS_ENUM_H_
#define SRC_TRAITS_ENUM_H_

#include <string>

#include "../utilities/Annotation.h"
#include "TraitData.h"

struct json_t;

class Enum: public Annotation
{
public:
  /**
   * Default constructor
   */
  Enum();

  /**
   * Copy construnctor
   * @param const Feature & src
   */
  Enum(const Enum & src);

  /**
   * Destructor
   */
  virtual ~Enum();

  void fromJSON(const json_t * json);

  const std::string & getId() const;

  const bool & isValid() const;

  void setMask(const TraitData & mask);

  const TraitData & getMask() const;

private:
  std::string mId;
  TraitData mMask;
  bool mValid;
};

#endif /* SRC_TRAITS_ENUM_H_ */
