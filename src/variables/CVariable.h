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

#ifndef SRC_VARIABLES_CVARIABLE_H_
#define SRC_VARIABLES_CVARIABLE_H_

#include "math/CComputable.h"
#include "utilities/CAnnotation.h"
#include "math/CValue.h"

class CMetadata;

class CVariable: public CValue, public CAnnotation
{
public:
  enum struct Scope
  {
    global,
    local
  };

public:
  static CVariable * transmissibility();
  
  CVariable();

  CVariable(const CVariable & src);

  CVariable(const json_t * json);

  virtual ~CVariable();

  virtual CValueInterface * copy() const override;

  virtual void fromJSON(const json_t * json) override;

  void toBinary(std::ostream & os) const;

  void fromBinary(std::istream & is);

  const std::string & getId() const;

  const bool & isValid() const;

  const Scope & getScope() const;
  
  void reset(const bool & force = false);

  void getValue();

  bool setValue(double value, CValueInterface::pOperator pOperator, const CMetadata & metadata);

  bool setValue(const CValue value, CValueInterface::pOperator pOperator, const CMetadata & metadata);

  void setInitialValue(const double & initialValue);

  /*
    "=",
    "*=",
    "/=",
    "+=",
    "-="
   */

private:
  std::string mId;
  Scope mScope;
  double mInitialValue;
  double * mpLocalValue;
  int mResetValue;
  size_t mIndex;
  bool mValid;
};

#endif /* SRC_VARIABLES_CVARIABLE_H_ */
