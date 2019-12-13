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

#ifndef SRC_INTERVENTION_CINITIALIZATION_H_
#define SRC_INTERVENTION_CINITIALIZATION_H_

#include <vector>

#include "utilities/CAnnotation.h"
#include "actions/CActionEnsemble.h"

class CSetContent;
struct json_t;

class CInitialization : public CAnnotation
{
public:
  static std::vector< CInitialization  * > INSTANCES;

  static void load(const std::string & file);

  static void release();

  static void processAll();

  CInitialization();

  CInitialization(const CInitialization & src);

  CInitialization(const json_t * json);

  virtual ~CInitialization();

  void fromJSON(const json_t * json);

  const bool & isValid() const;

  void process();

  CSetContent * getTarget();

private:
  CSetContent * mpTarget;
  CActionEnsemble mActionEnsemble;

protected:
  bool mValid;
};

#endif /* SRC_INTERVENTION_CINITIALIZATION_H_ */
