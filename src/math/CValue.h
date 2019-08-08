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

#ifndef SRC_MATH_CVALUE_H_
#define SRC_MATH_CVALUE_H_

#include "math/CValueInterface.h"

struct json_t;

class CValue: public CValueInterface
{
public:
  CValue() = delete;

  CValue(const bool & boolean);

  CValue(const double & number);

  CValue(const CTraitData::value & traitValue);

  CValue(const std::string & str);

  CValue(const size_t & id);

  CValue(const CValue & src);

  CValue(const json_t * json);

  virtual ~CValue();

  void fromJSON(const json_t * json);

  const bool & isValid() const;


private:
  static void * createValue(const Type & type);

  void assignValue(const void * pValue);

  void destroyValue();

  bool mValid;
};

#endif /* SRC_MATH_CVALUE_H_ */
