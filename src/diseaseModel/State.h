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

#ifndef SRC_DISEASEMODEL_STATE_H_
#define SRC_DISEASEMODEL_STATE_H_

#include <string>
#include "utilities/Annotation.h"

struct json_t;

class State: public Annotation
{
public:
  State();

  State(const State & src);

  virtual ~State();

  void fromJSON(const json_t * json);

  const std::string & getId() const;

  const bool & isValid() const;

private:
  std::string mId;
  double mSusceptibility;
  double mInfectivity;
  bool mValid;
};

#endif /* SRC_DISEASEMODEL_STATE_H_ */
