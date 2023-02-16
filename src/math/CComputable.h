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

#ifndef SRC_MATH_CCOMPUTABLE_H_
#define SRC_MATH_CCOMPUTABLE_H_

#include <map>
#include <vector>
#include <string>

#include "math/CComputableSet.h"
#include "utilities/CContext.h"

class CComputable
{
  friend class CComputableSet;

public:
  typedef std::vector< CComputable * > Sequence;

  static CComputableSet Instances;

  CComputable();

  CComputable(const CComputable & src);

  virtual ~CComputable();

  const CComputableSet & getPrerequisites() const;

  bool compute();

  bool isStatic() const;

  virtual std::string getComputableId() const;

protected:
  void determineIsStatic();

  virtual bool computeProtected() = 0;
  
private:
  static size_t UniqueId;

  size_t mComputableId;

protected:
  bool mStatic;
  CContext< bool > mComputedOnce;
  CComputableSet mPrerequisites;
};

#endif /* SRC_MATH_CCOMPUTABLE_H_ */
