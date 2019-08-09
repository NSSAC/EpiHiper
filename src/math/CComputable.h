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

#include <set>
#include <vector>

class CComputable
{
public:
  typedef std::set< const CComputable * > Set;
  typedef std::vector< CComputable * > Sequence;

  CComputable();

  CComputable(const CComputable & src);

  virtual ~CComputable();

  const Set & getPrerequisites() const;

  virtual void compute() = 0;

protected:
  Set mPrerequisites;
};

#endif /* SRC_MATH_CCOMPUTABLE_H_ */
