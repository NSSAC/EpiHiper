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

#ifndef SRC_INTERVENTION_CVARIABLE_H_
#define SRC_INTERVENTION_CVARIABLE_H_

#include "utilities/CAnnotation.h"
#include "dependencies/CComputable.h"

class CVariable: public CAnnotation, public CComputable
{
public:
  enum struct Type
  {
    global,
    local
  };

  CVariable() = delete;

  CVariable(const CVariable & src);

  CVariable(const json_t * json);

  virtual ~CVariable();

  void fromJSON(const json_t * json);

  void toBinary(std::ostream & os) const;

  void fromBinary(std::istream & is);

  const std::string & getId() const;

  const bool & isValid() const;

  /*
    "=",
    "*=",
    "/=",
    "+=",
    "-="
   */

private:
  std::string mId;
  Type mType;
  double mValue;
  double mResetValue;
  bool mValid;
};

#endif /* SRC_INTERVENTION_CVARIABLE_H_ */
