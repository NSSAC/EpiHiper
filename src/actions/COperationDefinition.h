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
 * COperationDefinition.h
 *
 *  Created on: Aug 29, 2019
 *      Author: shoops
 */

#ifndef SRC_ACTIONS_COPERATIONDEFINITION_H_
#define SRC_ACTIONS_COPERATIONDEFINITION_H_

class COperation;

class COperationDefinition
{
public:
  enum struct TargetType
  {
    node,
    edge,
    variable
  };

  /**
   * Default constructor
   */
  COperationDefinition();

  /**
   * Copy construnctor
   * @param const Feature & src
   */
  COperationDefinition(const COperationDefinition & src);

  /**
   * Destructor
   */
  virtual ~COperationDefinition();

  void fromJSON(const json_t * json);

  template < class Target > COperation * createOperation(Target * pTarget) const;
};

template < class Target > COperation * COperationDefinition::createOperation(Target * pTarget) const
{
  // TODO CRITICAL Implement me!
  return NULL;
}

#endif /* SRC_ACTIONS_COPERATIONDEFINITION_H_ */
