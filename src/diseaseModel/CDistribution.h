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

#ifndef SRC_DISEASEMODEL_CDISTRIBUTION_H_
#define SRC_DISEASEMODEL_CDISTRIBUTION_H_

#include <vector>
#include <utility>

#include "utilities/CRandom.h"

struct json_t;

class CDistribution
{
public:
  enum struct Type {
   fixed,
   discrete,
   uniform,
   normal,
   gamma,
   __NONE
  };

  CDistribution();

  CDistribution(const CDistribution & src);

  virtual ~CDistribution();

  virtual void fromJSON(const json_t * json);

  const bool & isValid() const;

  unsigned int sample() const;

private:
  Type mType;
  std::vector< std::pair <double, unsigned int > > mDiscrete;
  std::vector< unsigned int > mUniformSet;
  unsigned int (CDistribution::*mpSample)() const;

  unsigned int mFixed;
  CRandom::uniform_int * mpUniformInt;
  CRandom::uniform_real * mpUniformReal;
  CRandom::normal * mpNormal;
  CRandom::gamma * mpGamma;

  bool mValid;

  unsigned int fixed() const;
  unsigned int discrete() const;
  unsigned int uniformSet() const;
  unsigned int uniformDiscrete() const;
  unsigned int normal() const;
  unsigned int gamma() const;
};

#endif /* SRC_DISEASEMODEL_CDISTRIBUTION_H_ */
