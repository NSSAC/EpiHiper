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

#include "math/CComputable.h"

// static
size_t CComputable::UniqueId(0);

CComputable::CComputable()
  : mComputableId(UniqueId++)
  , mPrerequisites()
{
  Instances.insert(this);
}

CComputable::CComputable(const CComputable & src)
  : mComputableId(UniqueId++)
  , mPrerequisites(src.mPrerequisites)
{
  Instances.insert(this);
}

CComputable::~CComputable()
{
  Instances.erase(this);
}

const CComputableSet & CComputable::getPrerequisites() const
{
  return mPrerequisites;
}

