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

#ifndef SRC_DISEASEMODEL_CTRANSMISSION_H_
#define SRC_DISEASEMODEL_CTRANSMISSION_H_

#include <map>

#include "utilities/CAnnotation.h"
#include "diseaseModel/CFactorOperation.h"
#include "plugins/CCustomMethod.h"

struct json_t;
class CHealthState;

class CTransmission: public CAnnotation, public CCustomMethod< CCustomMethodType::transmission_propensity >
{
public:
  static double defaultMethod(const CTransmission * pTransmission, const CEdge * pEdge);

  CTransmission();

  CTransmission(const CTransmission & src);

  virtual ~CTransmission();

  void fromJSON(const json_t * json, const std::map< std::string, CHealthState * > & states);

  const std::string & getId() const;

  const CHealthState * getEntryState() const;

  const CHealthState * getContactState() const;

  const CHealthState * getExitState() const;

  const double & getTransmissibility() const;

  const bool & isValid() const;

  void updateSusceptibilityFactor(double & factor) const;

  void updateInfectivityFactor(double & factor) const;

  double propensity(const CEdge * pEdge) const;

private:
  virtual void fromJSON(const json_t * json) override;
  
  std::string mId;
  const CHealthState * mpEntryState;
  const CHealthState * mpExitState;
  const CHealthState * mpContactState;
  double mTransmissibility;
  CFactorOperation mSusceptibilityFactorOperation;
  CFactorOperation mInfectivityFactorOperation;
  bool mValid;
};

#endif /* SRC_DISEASEMODEL_CTRANSMISSION_H_ */
