// BEGIN: Copyright 
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

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

  virtual CSetContent * copy() const override;

  const std::string & getId() const;

  const bool & isValid() const;

  virtual void fromJSON(const json_t * json) override;

  virtual bool computeProtected() override;

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

private:
  std::string mId;
  Type mType;
  CSetContent * mpSetContent;
  bool mValid;
};

#endif /* SRC_INTERVENTION_CSET_H_ */
