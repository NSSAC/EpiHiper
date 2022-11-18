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

#ifndef SRC_SETS_CSETREFERENCE_H_
#define SRC_SETS_CSETREFERENCE_H_

#include "sets/CSetContent.h"
#include "sets/CSet.h"

struct json_t;
class CSetReference : public CSetContent
{
private:
  static std::vector< CSetReference * > UnResolved;

public:
  static bool resolve();

  CSetReference();

  CSetReference(const CSetReference & src);

  CSetReference(const json_t * json);

  virtual ~CSetReference();

  virtual CSetContent * copy() const override;

  virtual void fromJSON(const json_t * json) override;

  virtual bool computeProtected() override;

  virtual std::string getComputableId() const override;

  virtual const CContext< SetContent > & getContext() const override
  {
    if (mValid)
      return mpSet->getContext();

    return CSetContent::getContext();
  }

  virtual CContext< SetContent > & getContext() override
  {
    if (mValid)
      return mpSet->getContext();

    return CSetContent::getContext();
  }

private:
  std::string mIdRef;
  CSet * mpSet;
};

#endif /* SRC_SETS_CSETREFERENCE_H_ */
