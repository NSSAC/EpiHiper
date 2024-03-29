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

  bool setTargetActivity(const CTraitData::value & value, CValueInterface::pOperator pOperator, const CMetadata & metadata);
  bool setSourceActivity(const CTraitData::value & value, CValueInterface::pOperator pOperator, const CMetadata & metadata);
  bool setEdgeTrait(const CTraitData::value & value, CValueInterface::pOperator pOperator, const CMetadata & metadata);
  bool setActive(const bool & value, CValueInterface::pOperator pOperator, const CMetadata & metadata);
  bool setWeight(const double & value, CValueInterface::pOperator pOperator, const CMetadata & metadata);

  // start binary data
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
  // end binary data

  CNode * pTarget;
  CNode * pSource;
};

#endif /* SRC_NETWORK_CEDGE_H_ */
