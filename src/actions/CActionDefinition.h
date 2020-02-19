// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#ifndef SRC_ACTIONS_CACTIONDEFINITION_H_
#define SRC_ACTIONS_CACTIONDEFINITION_H_

#include <vector>

#include "utilities/CAnnotation.h"
#include "utilities/CMetadata.h"
#include "actions/COperationDefinition.h"
#include "actions/CConditionDefinition.h"

class CActionDefinition : public CAnnotation
{
public:

  CActionDefinition();

  CActionDefinition(const CActionDefinition & src);

  CActionDefinition(const json_t * json);

  virtual ~CActionDefinition();

  virtual void fromJSON(const json_t * json);

  const bool & isValid() const;

  void process() const;

  void process(const CEdge * pEdge) const;

  void process(const CNode * pNode) const;

  static CActionDefinition * GetActionDefinition(const size_t & index);

private:
  static std::vector< CActionDefinition * > INSTANCES;

  std::vector< COperationDefinition > mOperations;
  double mPriority;
  size_t mDelay;
  CConditionDefinition mCondition;
  size_t mIndex;
  bool mValid;
  CMetadata mInfo;
};


#endif /* SRC_ACTIONS_CACTIONDEFINITION_H_ */
