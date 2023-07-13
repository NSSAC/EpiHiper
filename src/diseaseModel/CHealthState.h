// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2023 Rector and Visitors of the University of Virginia 
//  
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal 
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions: 
//  
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software. 
//  
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE 
// END: Copyright 

#ifndef SRC_DISEASEMODEL_CHEALTHSTATE_H_
#define SRC_DISEASEMODEL_CHEALTHSTATE_H_

#include <string>
#include <vector>

#include "utilities/CAnnotation.h"
#include "utilities/CContext.h"
#include "plugins/CCustomMethod.h"
#include "math/CValueInterface.h"

class CProgression;
class pNode;
struct json_t;
class CMetadata;

class CHealthState: public CAnnotation, public CCustomMethod< CCustomMethodType::state_progression >
{
public:
  struct PossibleProgressions
  {
    double A0 = 0.0;
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

  std::string & getId();

  size_t & getIndex();
  
  double & getSusceptibility();

  double & getInfectivity();

  bool setSusceptibility(const double & value, CValueInterface::pOperator pOperator, const CMetadata & metadata);

  bool setInfectivity(const double & value, CValueInterface::pOperator pOperator, const CMetadata & metadata);

  const PossibleProgressions & getPossibleProgressions() const;

  void addProgression(const CProgression * pProgression);

  void updatePossibleProgression();

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
