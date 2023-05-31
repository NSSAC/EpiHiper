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
struct COperation;
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

  virtual void fromJSON(const json_t * json);

  bool operator != (const CNodeProperty & rhs) const;

  bool operator < (const CNodeProperty & rhs) const;

  const bool & isValid() const;

  CValueInterface propertyOf(const CNode * pNode) const;
  
  COperation * createOperation(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);

  bool execute(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);

  bool isReadOnly() const;

  static std::pair< CEdge *, CEdge * > edges(CNode * pNode);

  void registerSetCollector(std::shared_ptr< CSetCollectorInterface > pCollector) const;

  void deregisterSetCollector(std::shared_ptr< CSetCollectorInterface > pCollector) const;

private:
  CValueInterface id(CNode * pNode) const;
  CValueInterface susceptibilityFactor(CNode * pNode) const;
  CValueInterface infectivityFactor(CNode * pNode) const;
  CValueInterface healthState(CNode * pNode) const;
  CValueInterface nodeTrait(CNode * pNode) const;

  bool setId(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  bool setSusceptibilityFactor(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  bool setInfectivityFactor(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  bool setHealthState(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);
  bool setNodeTrait(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info);

  Property mProperty;
  CValueInterface (CNodeProperty::*mpPropertyOf)(CNode *) const;
  bool (CNodeProperty::*mpExecute)(CNode *, const CValueInterface &, CValueInterface::pOperator pOperator, const CMetadata & info);

  bool mValid;
};

#endif /* SRC_MATH_CNODEPROPERTY_H_ */
