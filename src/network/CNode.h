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

/*
 * Node.h
 *
 *  Created on: Jun 26, 2019
 *      Author: shoops
 */

#ifndef SRC_NETWORK_CNODE_H_
#define SRC_NETWORK_CNODE_H_

#include <iostream>

#include "traits/CTraitData.h"
#include "diseaseModel/CModel.h"
#include "math/CValueInterface.h"

class CHealthState;
class CTransmission;
class CProgression;
class CMetadata;

class CEdge;

class CNode
{
public:
  struct sOutgoingEdges
  {
    CEdge ** pEdges;
    size_t Size;
  };

  static CNode getDefault();

  CNode();
  CNode(const CNode & src);

  virtual ~CNode();

  CNode & operator = (const CNode & rhs);

  void toBinary(std::ostream & os) const;
  void fromBinary(std::istream & is);

  bool set(const CTransmission * pTransmission, CValueInterface::pOperator pOperator, const CMetadata & metadata);
  bool set(const CProgression * pProgression, CValueInterface::pOperator pOperator, const CMetadata & metadata);
  bool setSusceptibilityFactor(double value, CValueInterface::pOperator pOperator, const CMetadata & metadata);
  bool setInfectivityFactor(double value, CValueInterface::pOperator pOperator, const CMetadata & metadata);
  bool setHealthState(CModel::state_t value, CValueInterface::pOperator pOperator, const CMetadata & metadata);
  bool setNodeTrait(CTraitData::value value, CValueInterface::pOperator pOperator, const CMetadata & metadata);

  inline const CHealthState *const  & getHealthState() const {return pHealthState;}
  void setHealthState(const CHealthState * pHealthState);

  size_t id;
  CModel::state_t healthState;
  double susceptibilityFactor;
  double susceptibility;
  double infectivityFactor;
  double infectivity;
  CTraitData::base nodeTrait;
  CEdge * Edges;
  size_t EdgesSize;
  CContext< sOutgoingEdges > OutgoingEdges;

private:
  const CHealthState * pHealthState;
};

#endif /* SRC_NETWORK_CNODE_H_ */
