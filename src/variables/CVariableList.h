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
 * CVariableList.h
 *
 *  Created on: Aug 4, 2019
 *      Author: shoops
 */

#ifndef SRC_VARIABLES_CVARIABLELIST_H_
#define SRC_VARIABLES_CVARIABLELIST_H_

#include <vector>
#include <map>

#include "variables/CVariable.h"

struct json_t;

class CVariableList: protected std::vector< CVariable * >
{
private: 
  typedef std::vector< CVariable * > base;

public:
  typedef std::vector< CVariable * >::const_iterator const_iterator;

  static CVariableList INSTANCE;

  CVariableList();

  CVariableList(const CVariableList & src);

  virtual ~CVariableList();

  virtual void fromJSON(const json_t * json);

  void toBinary(std::ostream & os) const;

  void fromBinary(std::istream & is);

  CVariable & operator[](const size_t & id);

  CVariable & operator[](const std::string & id);

  CVariable & operator[](const json_t * json);

  void resetAll(const bool & force = false);

  bool append(CVariable * pVariable);

  const_iterator begin() const;
  
  const_iterator end() const;

private:
  std::map< std::string, size_t > mId2Index;
  bool mValid;
};

#endif /* SRC_VARIABLES_CVARIABLELIST_H_ */
