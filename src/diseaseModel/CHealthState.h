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

#ifndef SRC_DISEASEMODEL_CHEALTHSTATE_H_
#define SRC_DISEASEMODEL_CHEALTHSTATE_H_

#include <string>

#include "utilities/CAnnotation.h"

struct json_t;

class CHealthState: public CAnnotation
{
public:
  CHealthState();

  CHealthState(const CHealthState & src);

  virtual ~CHealthState();

  virtual void fromJSON(const json_t * json);

  const std::string & getId() const;

  const double & getSusceptibility() const;

  const double & getInfectivity() const;

  const bool & isValid() const;

  const size_t & getLocalCount() const;

  const size_t & getGlobalCount() const;

  inline void increment() const {++mLocalCount;};

  void decrement() const {--mLocalCount;};

  void resetGlobalCount();

  void incrementGlobalCount(const size_t & i);

private:
  std::string mId;
  double mSusceptibility;
  double mInfectivity;
  bool mValid;
  mutable size_t mLocalCount;
  size_t mGlobalCount;
};

#endif /* SRC_DISEASEMODEL_CHEALTHSTATE_H_ */
