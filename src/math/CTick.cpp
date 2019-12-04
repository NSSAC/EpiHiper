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

/*
 * CTick.cpp
 *
 *  Created on: Nov 22, 2019
 *      Author: shoops
 */

#include "math/CTick.h"

CTick::CTick(const int & value)
  : CComputable()
  , mValue(value)
{}

CTick::CTick(const CTick & src)
 : CComputable(src)
 , mValue(src.mValue)
{}

// virtual
CTick::~CTick()
{}

// virtual
void CTick::compute()
{}

CTick::operator int() const
{
  return mValue;
}

CTick & CTick::operator++()
{
  ++mValue;

  return *this;
}

CTick & CTick::operator = (const int & value)
{
  mValue = value;

  return *this;
}

