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

#ifndef SRC_MATH_COBSERVABLE_H_
#define SRC_MATH_COBSERVABLE_H_

#include "math/CComputable.h"
#include "math/CValueInterface.h"

struct json_t;
class CObservable : public CValueInterface, public CComputable
{
public:
  CObservable() = delete;

  CObservable(const CObservable & src);

  // CObservable(const json_t * json);

  virtual ~CObservable();

  virtual void compute();

  void fromJSON(const json_t * json);

  const bool & isValid() const;

private:
  bool mValid;
};

#endif /* SRC_MATH_COBSERVABLE_H_ */
