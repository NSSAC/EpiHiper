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

#ifndef SRC_VARIABLES_CVARIABLE_H_
#define SRC_VARIABLES_CVARIABLE_H_

#include "math/CComputable.h"
#include "utilities/CAnnotation.h"
#include "math/CValue.h"

class CMetadata;

class CVariable: public CValue, public CAnnotation
{
public:
  enum struct Type
  {
    global,
    local
  };

public:
  static CVariable Transmissibility();
  
  CVariable();

  CVariable(const CVariable & src);

  CVariable(const json_t * json);

  virtual ~CVariable();

  virtual CValueInterface * copy() const override;

  virtual void fromJSON(const json_t * json) override;

  void toBinary(std::ostream & os) const;

  void fromBinary(std::istream & is);

  const std::string & getId() const;

  const bool & isValid() const;

  const Type & getType() const;
  
  void reset(const bool & force = false);

  void getValue();

  bool setValue(double value, CValueInterface::pOperator pOperator, const CMetadata & metadata);

  bool setValue(const CValue value, CValueInterface::pOperator pOperator, const CMetadata & metadata);

  void setIndex(const size_t & index);

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
  double mInitialValue;
  double * mpLocalValue;
  int mResetValue;
  size_t mIndex;
  bool mValid;
};

#endif /* SRC_VARIABLES_CVARIABLE_H_ */
