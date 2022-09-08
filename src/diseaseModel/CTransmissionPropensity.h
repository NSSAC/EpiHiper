// BEGIN: Copyright 
// Copyright (C) 2020 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#ifndef SRC_DISEASEMODEL_CTRANSMISSIONPROPENSITY_H_
#define SRC_DISEASEMODEL_CTRANSMISSIONPROPENSITY_H_

#include <string>

class CEdge;
class CTransmission;

class CTransmissionPropensity {
public:
  typedef double (*calculate_type)(const CEdge * pEdge, const CTransmission * pTransmission);

  static double defaultPropensity(const CEdge * pEdge, const CTransmission * pTransmission);
  static void Init();
  static calculate_type calculate;
};

#endif // SRC_DISEASEMODEL_CTRANSMISSIONPROPENSITY_H_
