// BEGIN: Copyright 
// Copyright (C) 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#ifndef SRC_PLUGINS_CDWELLTIME_H_
#define SRC_PLUGINS_CDWELLTIME_H_

#include <string>

class CNode;
class CProgression;

class CDwellTime {
public:
  typedef unsigned int (*calculate_type)(const CProgression * pProgression, const CNode * pNode);
  static unsigned int defaultCalculate(const CProgression * pProgression, const CNode * pNode);
  static void Init();
  static calculate_type calculate;
};

#endif // SRC_PLUGINS_CDWELLTIME_H_
