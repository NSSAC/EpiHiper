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

#include <jansson.h>

#include "Condition.h"

Boolean::Boolean()
{}

Boolean::Boolean(const Boolean & src)
{}

// virtual
Boolean::~Boolean()
{}

Condition::Condition(const Boolean & boolean)
  : Boolean()
  , mpBoolean(boolean.copy())
{}

Condition::Condition(const Condition & src)
  : Boolean(src)
  , mpBoolean(src.mpBoolean->copy())
{}

// virtual
Condition::~Condition()
{
  if (mpBoolean != NULL) delete mpBoolean;
}

// virtual
Boolean * Condition::copy() const
{
  return new Condition(*this);
}

void Condition::fromJSON(const json_t * json)
{
  json_t * pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
    }
}

bool Condition::isTrue() const
{
  return mpBoolean->isTrue();
}
