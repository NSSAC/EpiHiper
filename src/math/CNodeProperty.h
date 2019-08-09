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

#ifndef SRC_MATH_CNODEPROPERTY_H_
#define SRC_MATH_CNODEPROPERTY_H_

#include "math/CValueInterface.h"

class CNode;
class CEdge;
class COperation;
struct json_t;

class CNodeProperty: public CValueInterface
{
public:
  CNodeProperty();

  virtual ~CNodeProperty();

  void fromJSON(const json_t * json);

  const bool & isValid() const;

  CValueInterface & propertyOf(CNode * pNode);
  COperation * createOperation(CNode * pNode, const CValueInterface & value);

  static std::pair< CEdge *, CEdge * > edges(CNode * pNode);

private:
  CValueInterface & id(CNode * pNode);
  CValueInterface & susceptibilityFactor(CNode * pNode);
  CValueInterface & infectivityFactor(CNode * pNode);
  CValueInterface & healthState(CNode * pNode);
  CValueInterface & nodeTrait(CNode * pNode);

  COperation * setId(CNode * pNode, const CValueInterface & value);
  COperation * setSusceptibilityFactor(CNode * pNode, const CValueInterface & value);
  COperation * setInfectivityFactor(CNode * pNode, const CValueInterface & value);
  COperation * setHealthState(CNode * pNode, const CValueInterface & value);
  COperation * setNodeTrait(CNode * pNode, const CValueInterface & value);

  CValueInterface & (CNodeProperty::*mpPropertyOf)(CNode *);
  COperation * (CNodeProperty::*mpCreateOperation)(CNode *, const CValueInterface &);

  CModel::state_t mHealthState;
  bool mValid;
};

#endif /* SRC_MATH_CNODEPROPERTY_H_ */
