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

#ifndef SRC_MATH_CNODEPROPERTY_H_
#define SRC_MATH_CNODEPROPERTY_H_

#include <memory>
#include "math/CValueInterface.h"

class CValue;
class CNode;
class CEdge;
class COperation;
class CMetadata;
struct json_t;
class CSetCollectorInterface;

class CNodeProperty: public CValueInterface
{
public:
  enum struct Property {
    id,
    susceptibilityFactor,
    infectivityFactor,
    healthState,
    nodeTrait,
    edges,
    __SIZE
  };

  static std::vector< std::set< std::shared_ptr< CSetCollectorInterface > > > Collectors;

  CNodeProperty();

  CNodeProperty(const CNodeProperty & src);

  CNodeProperty(const json_t * json);

  virtual ~CNodeProperty();

  virtual CValueInterface * copy() const override;

  virtual void fromJSON(const json_t * json);

  bool operator != (const CNodeProperty & rhs) const;

  bool operator < (const CNodeProperty & rhs) const;

  const bool & isValid() const;

  CValue propertyOf(const CNode * pNode);
  
  COperation * createOperation(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);

  bool isReadOnly() const;

  static std::pair< CEdge *, CEdge * > edges(CNode * pNode);

  void registerSetCollector(std::shared_ptr< CSetCollectorInterface > pCollector) const;

  void deregisterSetCollector(std::shared_ptr< CSetCollectorInterface > pCollector) const;

private:
  CValue id(CNode * pNode);
  CValue susceptibilityFactor(CNode * pNode);
  CValue infectivityFactor(CNode * pNode);
  CValue healthState(CNode * pNode);
  CValue nodeTrait(CNode * pNode);

  COperation * setId(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  COperation * setSusceptibilityFactor(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  COperation * setInfectivityFactor(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  COperation * setHealthState(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  COperation * setNodeTrait(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);

  Property mProperty;
  CValue (CNodeProperty::*mpPropertyOf)(CNode *);
  COperation * (CNodeProperty::*mpCreateOperation)(CNode *, const CValueInterface &, CValueInterface::pOperator pOperator, const CMetadata & info);

  bool mValid;
};

#endif /* SRC_MATH_CNODEPROPERTY_H_ */
