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

/*
 * Condition.h
 *
 *  Created on: Jul 1, 2019
 *      Author: shoops
 */

#ifndef SRC_ACTIONS_CONDITION_H_
#define SRC_ACTIONS_CONDITION_H_

struct json_t;

class Condition
{
public:
  /**
   * Default constructor
   */
  Condition();

  /**
   * Copy construnctor
   * @param const Condition & src
   */
  Condition(const Condition & src);

  /**
   * Destructor
   */
  virtual ~Condition();

  void fromJSON(const json_t * json);

  bool isTrue() const;
};

#endif /* SRC_ACTIONS_CONDITION_H_ */
