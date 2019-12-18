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
 * Enum.h
 *
 *  Created on: Jun 22, 2019
 *      Author: shoops
 */

#ifndef SRC_TRAITS_CENUM_H_
#define SRC_TRAITS_CENUM_H_

#include <string>

#include "utilities/CAnnotation.h"
#include "traits/CTraitData.h"

struct json_t;

class CEnum: public CAnnotation
{
public:
  /**
   * Default constructor
   */
  CEnum(const std::string & id = "");

  /**
   * Copy construnctor
   * @param const Feature & src
   */
  CEnum(const CEnum & src);

  /**
   * Destructor
   */
  virtual ~CEnum();

  virtual void fromJSON(const json_t * json);

  const std::string & getId() const;

  const bool & isValid() const;

  void setMask(const CTraitData::base & mask);

  const CTraitData::base & getMask() const;

private:
  std::string mId;
  CTraitData::base mMask;
  bool mValid;
};

#endif /* SRC_TRAITS_CENUM_H_ */
