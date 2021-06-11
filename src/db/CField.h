// BEGIN: Copyright 
// Copyright (C) 2019 - 2021 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

/*
 * CField.h
 *
 *  Created on: Aug 8, 2019
 *      Author: shoops
 */

#ifndef SRC_DB_CFIELD_H_
#define SRC_DB_CFIELD_H_

#include <string>

#include "math/CValue.h"

struct json_t;

class CField
{
public:
  CField();

  CField(const CField &src);

  virtual ~CField();

  virtual void fromJSON(const json_t * json);

  const std::string & getId() const;

  const bool & isValid() const;

  const CValue::Type & getType() const;

  bool isValidValue( const std::string & value) const;

private:
  std::string mId;
  std::string mLabel;
  CValue::Type mType;
  std::set< std::string > mValidValues;
  bool mValid;
};

#endif /* SRC_DB_CFIELD_H_ */
