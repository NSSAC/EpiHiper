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

#ifndef SRC_INTERVENTION_CTRIGGER_H_
#define SRC_INTERVENTION_CTRIGGER_H_

#include <vector>
#include <map>

#include "math/CComputable.h"
#include "utilities/CAnnotation.h"
#include "utilities/CCommunicate.h"
#include "actions/CConditionDefinition.h"

class CIntervention;
struct json_t;

class CTrigger : public CAnnotation
{
public:
  static std::vector< CTrigger * > INSTANCES;

  static bool * pGlobalTriggered;

  static CComputableSet RequiredTargets;

  static void loadJSON(const json_t * json);

  static void release();

  static bool processAll();

  static CCommunicate::ErrorCode receive(std::istream & is, int sender);

  CTrigger();

  CTrigger(const CTrigger & src);

  CTrigger(const json_t * json);

  virtual ~CTrigger();

  virtual void process();

  virtual void fromJSON(const json_t * json);

  void trigger(const bool & triggers);

  const bool & isValid() const;

private:
  CConditionDefinition mCondition;
  std::map< std::string, CIntervention * > mInterventions;
  bool mIsLocalTrue;
  bool mValid;
};

#endif /* SRC_INTERVENTION_CTRIGGER_H_ */
