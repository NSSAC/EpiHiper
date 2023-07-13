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

#ifndef SRC_MATH_CProgressionProperty_H_
#define SRC_MATH_CProgressionProperty_H_

#include <memory>

#include "math/CValueInterface.h"

class CProgression;
struct COperation;
class CMetadata;
struct json_t;

class CProgressionProperty : public CValueInterface
{
public:
  enum struct Property
  {
    id,
    entryState,
    exitState,
    propensity,
    dwellTime,
    susceptibilityFactorOperation,
    infectivityFactorOperation,
    __SIZE
  };

  CProgressionProperty();

  CProgressionProperty(const CProgressionProperty & src);

  CProgressionProperty(const json_t * json);

  virtual ~CProgressionProperty();

  virtual void fromJSON(const json_t * json);

  bool operator!=(const CProgressionProperty & rhs) const;

  bool operator<(const CProgressionProperty & rhs) const;

  const bool & isValid() const;

  CValueInterface propertyValue() const;

  bool execute(const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);

  bool isReadOnly() const;

private:
  static CValueInterface id(CProgression * pProgression);
  static CValueInterface entryState(CProgression * pProgression);
  static CValueInterface exitState(CProgression * pProgression);
  static CValueInterface propensity(CProgression * pProgression);
  static CValueInterface dwellTime(CProgression * pProgression);
  static CValueInterface susceptibilityFactorOperation(CProgression * pProgression);
  static CValueInterface infectivityFactorOperation(CProgression * pProgression);

  static bool setId(CProgression * pProgression, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  static bool setEntryState(CProgression * pProgression, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  static bool setExitState(CProgression * pProgression, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  static bool setPropensity(CProgression * pProgression, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  static bool setDwellTime(CProgression * pProgression, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  static bool setSusceptibilityFactorOperation(CProgression * pProgression, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  static bool setInfectivityFactorOperation(CProgression * pProgression, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);

  Property mProperty;
  CProgression * mpProgression;
  CValueInterface (*mpPropertyOf)(CProgression *);
  bool (*mpExecute)(CProgression *, const CValueInterface &, CValueInterface::pOperator pOperator, const CMetadata & info);
  bool mValid;
};

#endif /* SRC_MATH_CProgressionProperty_H_ */
