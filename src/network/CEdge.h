// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
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

#ifndef SRC_NETWORK_CEDGE_H_
#define SRC_NETWORK_CEDGE_H_

#include <iostream>

#include "EpiHiperConfig.h"
#include "traits/CTraitData.h"
#include "math/CValueInterface.h"

class CNode;
class CMetadata;

class CEdge
{
public:
  static bool HasLocationId;
  static bool HasEdgeTrait;
  static bool HasActiveField;
  static bool HasWeightField;
  static CEdge getDefault();

  CEdge();
  virtual ~CEdge();

  void toBinary(std::ostream & os) const;
  void fromBinary(std::istream & is);

  bool setTargetActivity(CTraitData::value value, CValueInterface::pOperator pOperator, const CMetadata & metadata);
  bool setSourceActivity(CTraitData::value value, CValueInterface::pOperator pOperator, const CMetadata & metadata);
  bool setEdgeTrait(CTraitData::value value, CValueInterface::pOperator pOperator, const CMetadata & metadata);
  bool setActive(bool value, CValueInterface::pOperator pOperator, const CMetadata & metadata);
  bool setWeight(double value, CValueInterface::pOperator pOperator, const CMetadata & metadata);

  size_t targetId;
  CTraitData::base targetActivity;
  size_t sourceId;
  CTraitData::base sourceActivity;
  double duration;
#ifdef USE_LOCATION_ID
  size_t locationId;
#endif
  CTraitData::base edgeTrait;
  bool active;
  double weight;
  CNode * pTarget;
  CNode * pSource;
};

#endif /* SRC_NETWORK_CEDGE_H_ */
