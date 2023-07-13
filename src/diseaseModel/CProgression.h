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

#ifndef SRC_DISEASEMODEL_CPROGRESSION_H_
#define SRC_DISEASEMODEL_CPROGRESSION_H_

#include <map>

#include "utilities/CAnnotation.h"
#include "diseaseModel/CDistribution.h"
#include "diseaseModel/CFactorOperation.h"
#include "plugins/CCustomMethod.h"
#include "math/CValueInterface.h"

struct json_t;
class CHealthState;
class CMetadata;

class CProgression: public CAnnotation, public CCustomMethod< CCustomMethodType::progression_dwell_time >
{
public:
  static unsigned int defaultMethod(const CProgression * pProgression, const CNode * pNode);

  CProgression();

  CProgression(const CProgression & src);

  virtual ~CProgression();

  void fromJSON(const json_t * json, const std::map< std::string, CHealthState * > & states);

  const std::string & getId() const;

  const CHealthState * getEntryState() const;

  const CHealthState * getExitState() const;

  const double & getPropensity() const;

  std::string & getId();

  CHealthState * getEntryState();

  CHealthState * getExitState();

  double & getPropensity();

  std::string & getDwellTime();

  std::string & getSusceptibilityFactorOperation();

  std::string & getInfectivityFactorOperation();

  bool setPropensity(const double & value, CValueInterface::pOperator pOperator, const CMetadata & metadata);

  bool setDwellTime(const std::string & value, CValueInterface::pOperator pOperator, const CMetadata & metadata);

  bool setSusceptibilityFactorOperation(const std::string & value, CValueInterface::pOperator pOperator, const CMetadata & metadata);

  bool setInfectivityFactorOperation(const std::string & value, CValueInterface::pOperator pOperator, const CMetadata & metadata);

  unsigned int dwellTime(const CNode * pNode) const;

  void updateSusceptibilityFactor(double & factor) const;

  void updateInfectivityFactor(double & factor) const;

  const bool & isValid() const;

private:
  virtual void fromJSON(const json_t * json) override;
  
  std::string mId;
  const CHealthState * mpEntryState;
  const CHealthState * mpExitState;
  double mPropensity;
  CDistribution mDwellTime;
  CFactorOperation mSusceptibilityFactorOperation;
  CFactorOperation mInfectivityFactorOperation;
  bool mValid;
};

#endif /* SRC_DISEASEMODEL_CPROGRESSION_H_ */
