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

#ifndef SRC_DISEASEMODEL_CHEALTHSTATE_H_
#define SRC_DISEASEMODEL_CHEALTHSTATE_H_

#include <string>

#include "utilities/CAnnotation.h"
#include "utilities/CContext.h"

struct json_t;

class CHealthState: public CAnnotation
{
public:
  struct Counts
  {
    size_t Current;
    size_t In;
    size_t Out;
  };

  CHealthState();

  CHealthState(const CHealthState & src);

  virtual ~CHealthState();

  virtual void fromJSON(const json_t * json) override;

  const std::string & getId() const;

  const double & getSusceptibility() const;

  const double & getInfectivity() const;

  const bool & isValid() const;

  const CContext< Counts > & getLocalCounts() const;

  CContext< Counts > & getLocalCounts();

  const Counts & getGlobalCounts() const;

  inline void increment() const {
    Counts & Active = mLocalCounts.Active();
    ++Active.Current;
    ++Active.In;
  };

  inline void decrement() const {
    Counts & Active = mLocalCounts.Active();
    --Active.Current;
    ++Active.Out;
  };

  void setGlobalCounts(const Counts & counts);

  void incrementGlobalCount(const Counts & i);

private:
  std::string mId;
  double mSusceptibility;
  double mInfectivity;
  bool mValid;
  mutable CContext< Counts > mLocalCounts;
  Counts mGlobalCounts;
};

#endif /* SRC_DISEASEMODEL_CHEALTHSTATE_H_ */
