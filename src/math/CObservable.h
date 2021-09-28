// BEGIN: Copyright 
// Copyright (C) 2019 - 2021 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

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
