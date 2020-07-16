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

#ifndef SRC_INTERVENTION_CINTERVENTION_H_
#define SRC_INTERVENTION_CINTERVENTION_H_

#include <map>

#include "intervention/CInitialization.h"

class CIntervention : public CInitialization
{
public:
  static std::map< std::string, CIntervention * > INSTANCES;

  static void load(const std::string & file);

  static void release();

  static bool processAll();

  static CIntervention * getIntervention(const std::string & id);

  CIntervention();

  CIntervention(const CIntervention & src);

  CIntervention(const json_t * json);

  virtual ~CIntervention();

  virtual void fromJSON(const json_t * json) override;

  bool process();

  void trigger();

private:
  std::string mId;
  size_t mIsTriggered;
};

#endif /* SRC_INTERVENTION_CINTERVENTION_H_ */
