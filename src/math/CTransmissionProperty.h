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

#ifndef SRC_MATH_CTransmissionProperty_H_
#define SRC_MATH_CTransmissionProperty_H_

#include <memory>

#include "math/CValueInterface.h"

class CTransmission;
struct COperation;
class CMetadata;
struct json_t;

class CTransmissionProperty : public CValueInterface
{
public:
  enum struct Property
  {
    id,
    entryState,
    exitState,
    contactState,
    transmissibility,
    susceptibilityFactorOperation,
    infectivityFactorOperation,
    __SIZE
  };

  CTransmissionProperty();

  CTransmissionProperty(const CTransmissionProperty & src);

  CTransmissionProperty(const json_t * json);

  virtual ~CTransmissionProperty();

  virtual void fromJSON(const json_t * json);

  bool operator!=(const CTransmissionProperty & rhs) const;

  bool operator<(const CTransmissionProperty & rhs) const;

  const bool & isValid() const;

  CValueInterface propertyValue() const;

  bool execute(const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);

  bool isReadOnly() const;

private:
  static CValueInterface id(CTransmission * pTransmission);
  static CValueInterface entryState(CTransmission * pTransmission);
  static CValueInterface exitState(CTransmission * pTransmission);
  static CValueInterface contactState(CTransmission * pTransmission);
  static CValueInterface transmissibility(CTransmission * pTransmission);
  static CValueInterface susceptibilityFactorOperation(CTransmission * pTransmission);
  static CValueInterface infectivityFactorOperation(CTransmission * pTransmission);

  static bool setId(CTransmission * pTransmission, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  static bool setEntryState(CTransmission * pTransmission, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  static bool setExitState(CTransmission * pTransmission, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  static bool setContactState(CTransmission * pTransmission, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  static bool setTransmissibility(CTransmission * pTransmission, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  static bool setSusceptibilityFactorOperation(CTransmission * pTransmission, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  static bool setInfectivityFactorOperation(CTransmission * pTransmission, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);

  Property mProperty;
  CTransmission * mpTransmission;
  CValueInterface (*mpPropertyOf)(CTransmission *);
  bool (*mpExecute)(CTransmission *, const CValueInterface &, CValueInterface::pOperator pOperator, const CMetadata & info);
  bool mValid;
};

#endif /* SRC_MATH_CTransmissionProperty_H_ */
