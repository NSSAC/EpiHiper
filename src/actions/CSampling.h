// BEGIN: Copyright 
// Copyright (C) 2019 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#ifndef SRC_ACTIONS_CSAMPLING_H_
#define SRC_ACTIONS_CSAMPLING_H_

struct json_t;

class CSetContent;
class CActionEnsemble;

class CSampling
{
public:
  enum struct Type
  {
    relativeIndividual,
    relativeGroup,
    absolute,
  };

  CSampling();

  CSampling(const CSampling & src);

  CSampling(const json_t * json);

  virtual ~CSampling();

  virtual void fromJSON(const json_t *json);

  const bool & isValid() const;

  void process(const CSetContent & targets) const;

private:
  Type mType;
  CActionEnsemble * mpSampled;
  CActionEnsemble * mpNodeSampled;
  bool mValid;
};

#endif /* SRC_ACTIONS_CSAMPLING_H_ */
