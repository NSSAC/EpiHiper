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

  CValue propertyOf(const CEdge * pEdge);

  COperation * createOperation(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);

  bool isReadOnly() const;

  static CNode * targetNode(CEdge * pEdge);

  static CNode * sourceNode(CEdge * pEdge);

private:
  CValue targetId(CEdge * pEdge);
  CValue sourceId(CEdge * pEdge);
  CValue targetActivity(CEdge * pEdge);
  CValue sourceActivity(CEdge * pEdge);
  CValue locationId(CEdge * pEdge);
  CValue edgeTrait(CEdge * pEdge);
  CValue active(CEdge * pEdge);
  CValue weight(CEdge * pEdge);
  CValue duration(CEdge * pEdge);

  COperation * setTargetId(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  COperation * setSourceId(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  COperation * setTargetActivity(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  COperation * setSourceActivity(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  COperation * setLocationId(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  COperation * setEdgeTrait(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  COperation * setActive(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  COperation * setWeight(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  COperation * setDuration(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);

  CValue (CEdgeProperty::*mpPropertyOf)(CEdge *);
  COperation * (CEdgeProperty::*mpCreateOperation)(CEdge *, const CValueInterface &, CValueInterface::pOperator pOperator, const CMetadata & info);
  CNodeProperty mNodeProperty;
  bool mValid;
};

#endif /* SRC_MATH_CEDGEPROPERTY_H_ */
