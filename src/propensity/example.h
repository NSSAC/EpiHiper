// BEGIN: Copyright 
// Copyright (C) 2020 - 2021 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#ifndef SRC_PROPENSITY_EXAMPLE_H_
#define SRC_PROPENSITY_EXAMPLE_H_

class CTransmission;
class CEdge;

extern "C"
{
  double propensity(const CEdge * pEdge, const CTransmission * pTransmission);
}

#endif // SRC_PROPENSITY_EXAMPLE_H_