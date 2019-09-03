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

struct json_t;

#ifndef SRC_ACTIONS_CCONDITIONDEFINITION_H_
#define SRC_ACTIONS_CCONDITIONDEFINITION_H_

class CConditionDefinition
{
public:
  enum struct ComparisonType
  {
    Equal,
    NotEqual,
    Less,
    LessOrEqual,
    Greater,
    GreaterOrEqual
  };

  enum struct BooleanOperationType
  {
    And,
    Or,
    Not
  };

  /**
   * Default constructor
   */
  CConditionDefinition();

  /**
   * Copy construnctor
   * @param const CConditionDefinition & src
   */
  CConditionDefinition(const CConditionDefinition & src);

  CConditionDefinition(const json_t * json);

  /**
   * Destructor
   */
  virtual ~CConditionDefinition();

  void fromJSON(const json_t * json);

  const bool & isValid() const;

private:
  bool mValid;
};

#endif /* SRC_ACTIONS_CCONDITIONDEFINITION_H_ */
