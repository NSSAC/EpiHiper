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

#ifndef SRC_MATH_CCOMPUTABLE_H_
#define SRC_MATH_CCOMPUTABLE_H_

#include <map>
#include <vector>

#include "math/CComputableSet.h"

class CComputable
{
  friend class CComputableSet;

public:
  typedef std::vector< CComputable * > Sequence;

  static CComputableSet Instances;

  CComputable();

  CComputable(const CComputable & src);

  virtual ~CComputable();

  const CComputableSet & getPrerequisites() const;

  void compute();

  bool isStatic() const;

protected:
  void determineIsStatic();

  virtual void computeProtected() = 0;

private:
  static size_t UniqueId;

  size_t mComputableId;

protected:
  bool mStatic;
  bool mComputedOnce;
  CComputableSet mPrerequisites;
};

#endif /* SRC_MATH_CCOMPUTABLE_H_ */
