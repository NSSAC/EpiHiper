// BEGIN: Copyright 
// Copyright (C) 2019 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#ifndef SRC_MATH_CEDGEPROPERTY_H_
#define SRC_MATH_CEDGEPROPERTY_H_

#include "math/CValueInterface.h"
#include "math/CNodeProperty.h"

class CEdge;
class COperation;
struct json_t;

class CEdgeProperty: public CValueInterface
{
public:
  typedef CNode * (*pGetNode)(CEdge *);

  CEdgeProperty();

  virtual ~CEdgeProperty();

  void fromJSON(const json_t * json);

  const bool & isValid() const;

  CValueInterface & propertyOf(CEdge * pEdge);

  COperation * createOperation(CEdge * pEdge, const CValueInterface & value);

  static CNode * targetNode(CEdge * pEdge);

  static CNode * sourceNode(CEdge * pEdge);

private:
  CValueInterface & targetActivity(CEdge * pEdge);
  CValueInterface & sourceActivity(CEdge * pEdge);
  CValueInterface & edgeTrait(CEdge * pEdge);
  CValueInterface & active(CEdge * pEdge);
  CValueInterface & weight(CEdge * pEdge);

  COperation * setTargetActivity(CEdge * pEdge, const CValueInterface & value);
  COperation * setSourceActivity(CEdge * pEdge, const CValueInterface & value);
  COperation * setEdgeTrait(CEdge * pEdge, const CValueInterface & value);
  COperation * setActive(CEdge * pEdge, const CValueInterface & value);
  COperation * setWeight(CEdge * pEdge, const CValueInterface & value);

  CValueInterface & (CEdgeProperty::*mpPropertyOf)(CEdge *);
  COperation * (CEdgeProperty::*mpCreateOperation)(CEdge *, const CValueInterface &);
  CNodeProperty mNodeProperty;
  bool mValid;
};

#endif /* SRC_MATH_CEDGEPROPERTY_H_ */
