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

#ifndef SRC_INTERVENTION_CSET_H_
#define SRC_INTERVENTION_CSET_H_

#include <set>
#include <map>

#include "utilities/CAnnotation.h"
#include "math/CComputable.h"

class CSetContent;

class CSet: public CAnnotation
{
public:
  enum struct Type
  {
    global,
    local
  };

  CSet() = delete;

  CSet(const CSet & src);

  CSet(const json_t * json);

  virtual ~CSet();

  const std::string & getId() const;

  const bool & isValid() const;

  CSetContent * getSetContent() const;

  virtual void fromJSON(const json_t * json);

private:
  std::string mId;
  Type mType;
  CSetContent * mpSetContent;
  bool mValid;
};

#endif /* SRC_INTERVENTION_CSET_H_ */