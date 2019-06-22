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

#include "Distribution.h"

Distribution::Distribution()
  : mType()
  , mValid(false)
{}

Distribution::Distribution(const Distribution & src)
  : mType(src.mType)
  , mValid(src.mValid)
{}

// virtual
Distribution::~Distribution()
{}

void Distribution::fromJSON(const json_t * json)
{}

const bool & Distribution::isValid() const
{
  return mValid;
}
