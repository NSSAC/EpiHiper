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
