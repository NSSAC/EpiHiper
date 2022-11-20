// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
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
