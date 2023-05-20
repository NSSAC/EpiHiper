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

#ifndef SRC_INTERVENTION_CSET_H_
#define SRC_INTERVENTION_CSET_H_

#include <set>
#include <map>

#include "utilities/CAnnotation.h"
#include "math/CComputable.h"

#include "sets/CSetContent.h"

class CSet : public CSetContent, public CAnnotation
{
public:
  enum struct Type
  {
    global,
    local
  };

  static CSet * empty();

private:
  CSet();

public:
  CSet(const CSet & src);

  CSet(const json_t * json);

  virtual ~CSet();

  const std::string & getId() const;

  virtual std::string getComputableId() const override;
  
  inline virtual const CContext< SetContent > & getContext() const override
  {
    if (mValid && mpSetContent != NULL)
      return mpSetContent->getContext();

    return CSetContent::getContext();
  }

  inline virtual CContext< SetContent > & getContext() override
  {
    if (mValid && mpSetContent != NULL)
      return mpSetContent->getContext();

    return CSetContent::getContext();
  }

protected:
  virtual bool computeProtected() override;

  virtual void fromJSONProtected(const json_t * json) override;

  virtual bool lessThanProtected(const CSetContent & rhs) const override;

private:
  std::string mId;
  Type mType;
  shared_pointer mpSetContent;
};

#endif /* SRC_INTERVENTION_CSET_H_ */
