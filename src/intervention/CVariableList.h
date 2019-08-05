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
 * CVariableList.h
 *
 *  Created on: Aug 4, 2019
 *      Author: shoops
 */

#ifndef SRC_INTERVENTION_CVARIABLELIST_H_
#define SRC_INTERVENTION_CVARIABLELIST_H_

#include <vector>
#include <map>
#include "intervention/CVariable.h"

struct json_t;

class CVariableList: protected std::vector< CVariable>
{
public:
  CVariableList();

  CVariableList(const CVariableList & src);

  virtual ~CVariableList();

  void fromJSON(const json_t * json);

  void toBinary(std::ostream & os) const;

  void fromBinary(std::istream & is);

  CVariable & operator[](const size_t & id);

  CVariable & operator[](const std::string & id);

private:
  std::map< std::string, size_t > mId2Index;
  bool mValid;
};

#endif /* SRC_INTERVENTION_CVARIABLELIST_H_ */
