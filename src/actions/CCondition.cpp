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

#include "actions/CCondition.h"

#include <jansson.h>


CBoolean::CBoolean()
{}

CBoolean::CBoolean(const CBoolean & src)
{}

// virtual
CBoolean::~CBoolean()
{}

CCondition::CCondition(const CBoolean & boolean)
  : CBoolean()
  , mpBoolean(boolean.copy())
{}

CCondition::CCondition(const CCondition & src)
  : CBoolean(src)
  , mpBoolean(src.mpBoolean->copy())
{}

// virtual
CCondition::~CCondition()
{
  if (mpBoolean != NULL) delete mpBoolean;
}

// virtual
CBoolean * CCondition::copy() const
{
  return new CCondition(*this);
}

bool CCondition::isTrue() const
{
  return mpBoolean->isTrue();
}
