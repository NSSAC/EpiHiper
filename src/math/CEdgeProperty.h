// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
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
class CMetadata;
struct json_t;

class CEdgeProperty: public CValueInterface
{
public:
  typedef CNode * (*pGetNode)(CEdge *);

  CEdgeProperty();

  CEdgeProperty(const CEdgeProperty & src);

  CEdgeProperty(const json_t * json);

  virtual ~CEdgeProperty();

  virtual CValueInterface * copy() const override;

  virtual void fromJSON(const json_t * json);

  const bool & isValid() const;

  CValueInterface & propertyOf(const CEdge * pEdge);

  COperation * createOperation(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);

  bool isReadOnly() const;

  static CNode * targetNode(CEdge * pEdge);

  static CNode * sourceNode(CEdge * pEdge);

private:
  CValueInterface & targetId(CEdge * pEdge);
  CValueInterface & sourceId(CEdge * pEdge);
  CValueInterface & targetActivity(CEdge * pEdge);
  CValueInterface & sourceActivity(CEdge * pEdge);
  CValueInterface & locationId(CEdge * pEdge);
  CValueInterface & edgeTrait(CEdge * pEdge);
  CValueInterface & active(CEdge * pEdge);
  CValueInterface & weight(CEdge * pEdge);
  CValueInterface & duration(CEdge * pEdge);

  COperation * setTargetId(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  COperation * setSourceId(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  COperation * setTargetActivity(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  COperation * setSourceActivity(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  COperation * setLocationId(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  COperation * setEdgeTrait(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  COperation * setActive(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  COperation * setWeight(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  COperation * setDuration(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);

  CValueInterface & (CEdgeProperty::*mpPropertyOf)(CEdge *);
  COperation * (CEdgeProperty::*mpCreateOperation)(CEdge *, const CValueInterface &, CValueInterface::pOperator pOperator, const CMetadata & info);
  CNodeProperty mNodeProperty;
  bool mValid;
};

#endif /* SRC_MATH_CEDGEPROPERTY_H_ */
