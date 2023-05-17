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

#include <memory>

#include "math/CValueInterface.h"
#include "math/CNodeProperty.h"

class CEdge;
class COperation;
class CMetadata;
struct json_t;
class CSetCollectorInterface;

class CEdgeProperty: public CValueInterface
{
public:
  typedef CNode * (*pGetNode)(CEdge *);

  enum struct Property {
    targetId,
    sourceId,
    targetActivity,
    sourceActivity,
    locationId,
    edgeTrait,
    active,
    weight,
    duration,
    __SIZE
  };

  CEdgeProperty();

  CEdgeProperty(const CEdgeProperty & src);

  CEdgeProperty(const json_t * json);

  virtual ~CEdgeProperty();

  virtual CValueInterface * copy() const override;

  virtual void fromJSON(const json_t * json);

  bool operator != (const CEdgeProperty & rhs) const;

  bool operator < (const CEdgeProperty & rhs) const;

  const bool & isValid() const;

  CValueInterface propertyOf(const CEdge * pEdge) const;

  COperation * createOperation(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);

  bool execute(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);

  bool isReadOnly() const;

  static CNode * targetNode(CEdge * pEdge);

  static CNode * sourceNode(CEdge * pEdge);

  void registerSetCollector(std::shared_ptr< CSetCollectorInterface > pCollector) const;

  void deregisterSetCollector(std::shared_ptr< CSetCollectorInterface > pCollector) const;

private:
  static std::vector< std::set< std::shared_ptr< CSetCollectorInterface > > > Collectors;

  CValueInterface targetId(CEdge * pEdge) const;
  CValueInterface sourceId(CEdge * pEdge) const;
  CValueInterface targetActivity(CEdge * pEdge) const;
  CValueInterface sourceActivity(CEdge * pEdge) const;
  CValueInterface locationId(CEdge * pEdge) const;
  CValueInterface edgeTrait(CEdge * pEdge) const;
  CValueInterface active(CEdge * pEdge) const;
  CValueInterface weight(CEdge * pEdge) const;
  CValueInterface duration(CEdge * pEdge) const;

  bool setTargetId(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  bool setSourceId(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  bool setTargetActivity(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  bool setSourceActivity(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  bool setLocationId(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  bool setEdgeTrait(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  bool setActive(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  bool setWeight(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  bool setDuration(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);

  Property mProperty;
  CValueInterface (CEdgeProperty::*mpPropertyOf)(CEdge *) const;
  bool (CEdgeProperty::*mpExecute)(CEdge *, const CValueInterface &, CValueInterface::pOperator pOperator, const CMetadata & info);
  bool mValid;
};

#endif /* SRC_MATH_CEDGEPROPERTY_H_ */
