// BEGIN: Copyright 
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
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
#include <vector>

#include "utilities/CAnnotation.h"
#include "utilities/CContext.h"
#include "plugins/CCustomMethod.h"

class CProgression;
class pNode;
struct json_t;

class CHealthState: public CAnnotation, public CCustomMethod< CCustomMethodType::state_progression >
{
public:
  struct PossibleProgressions
  {
    double A0;
    std::vector< const CProgression * > Progressions;
  };

  struct Counts
  {
    size_t Current;
    size_t In;
    size_t Out;
  };

  static const CProgression * defaultMethod(const CHealthState * pHealthState, const CNode * pNode);

  CHealthState();

  CHealthState(const CHealthState & src);

  virtual ~CHealthState();

  virtual void fromJSON(const json_t * json) override;

  const bool & isValid() const;

  const std::string & getId() const;

  void setIndex(const size_t & index);

  const size_t & getIndex() const;

  const double & getSusceptibility() const;

  const double & getInfectivity() const;

  const PossibleProgressions & getPossibleProgressions() const;

  void addProgression(const CProgression * pProgression);

  const CProgression * nextProgression(const CNode * pNode) const;

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
  size_t mIndex;
  double mSusceptibility;
  double mInfectivity;
  bool mValid;
  mutable CContext< Counts > mLocalCounts;
  Counts mGlobalCounts;
  PossibleProgressions mProgressions;

};

#endif /* SRC_DISEASEMODEL_CHEALTHSTATE_H_ */
