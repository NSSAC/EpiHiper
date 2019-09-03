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

#include "actions/CSampling.h"

CSampling::CSampling()
  : mValid(false)
{}

CSampling::CSampling(const CSampling & src)
  : mValid(src.mValid)
{}

CSampling::CSampling(const json_t * json)
  : mValid(false)
{
  fromJSON(json);
}

// virtual
CSampling::~CSampling()
{}

// virtual
void CSampling::fromJSON(const json_t *json)
{

}

const bool & CSampling::isValid() const
{
  return mValid;
}
