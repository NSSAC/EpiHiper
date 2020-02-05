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

#ifndef SRC_INTERVENTION_CSETLIST_H_
#define SRC_INTERVENTION_CSETLIST_H_

#include <vector>
#include <map>

#include "sets/CSet.h"

class CSetList: protected std::vector< CSet * >
{
public:
  static CSetList INSTANCE;

  CSetList();

  CSetList(const CSetList & src);

  virtual ~CSetList();

  virtual void fromJSON(const json_t * json);

  void toBinary(std::ostream & os) const;

  void fromBinary(std::istream & is);

  CSet * operator[](const size_t & id);

  CSet * operator[](const std::string & id);

private:
  std::map< std::string, size_t > mId2Index;
  bool mValid;
};

#endif /* SRC_INTERVENTION_CSETLIST_H_ */
