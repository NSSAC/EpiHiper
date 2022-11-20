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

#ifndef SRC_MATH_COBSERVABLE_H_
#define SRC_MATH_COBSERVABLE_H_

#include <map>
#include <utility>

#include "math/CComputable.h"
#include "math/CValue.h"

struct json_t;
class CObservable : public CValue, public CComputable
{
public:
  enum struct ObservableType
  {
    time,
    healthStateAbsolute,
    healthStateRelative,
    totalPopulation
  };

  enum struct ObservableSubset
  {
    in,
    current,
    out
  };

  struct ObservableKey {
    size_t id;
    ObservableType type;
    ObservableSubset subset;

    bool operator < (const ObservableKey & rhs) const;
  };

  static CObservable * get(const ObservableKey & key);

  static CObservable * get(const json_t * json);

  CObservable();

  CObservable(const CObservable & src);

  virtual ~CObservable();

  virtual bool computeProtected() override;

  bool operator < (const CObservable & rhs) const;

private:
  typedef std::map< ObservableKey, CObservable * > ObservableMap;
  static ObservableMap Observables;

  // CObservable(const ObservableType & observableType, const size_t & id);

  CObservable(const json_t * json);

  virtual void fromJSON(const json_t * json) override;

  bool computeTime();

  bool computeTotalPopulation();

  bool computeHealthStateAbsolute();

  bool computeHealthStateRelative();

  ObservableType mObservableType;
  ObservableSubset mObservableSubset;
  size_t mId;

  bool (CObservable::*mpCompute)();

};

#endif /* SRC_MATH_COBSERVABLE_H_ */
