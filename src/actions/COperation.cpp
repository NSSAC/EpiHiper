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
 * Operation.cpp
 *
 *  Created on: Jul 1, 2019
 *      Author: shoops
 */

#include <jansson.h>

#include "actions/COperation.h"

COperationDefinition::COperationDefinition()
{}

COperationDefinition::COperationDefinition(const COperationDefinition & src)
{}

// virtual
COperationDefinition::~COperationDefinition()
{}

void COperationDefinition::fromJSON(const json_t * json)
{
  json_t * pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
    }

}


