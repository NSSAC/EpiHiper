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

#include "utilities/CAnnotation.h"
#include "math/CValueList.h"

class COperation;
class CNodeProperty;
class CEdgeProperty;
class CVariable;
class CNode;
class CEdge;
struct json_t;

class COperationDefinition : public CAnnotation
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

  COperationDefinition(const json_t * json);

  /**
   * Destructor
   */
  virtual ~COperationDefinition();

  void fromJSON(const json_t * json);

  const bool & isValid() const;

  COperation * createOperation(CNode * pNode) const;

  COperation * createOperation(CEdge * pEdge) const;

  COperation * createOperation(CVariable * pVariable) const;

private:
  TargetType mTargetType;
  CNodeProperty * mpNodeProperty;
  CEdgeProperty * mpEdgeProperty;
  CVariable * mpVariable;
  CValueInterface::pOperator mpOperator;
  CValue mValue;
  bool mValid;
};
#endif /* SRC_ACTIONS_COPERATIONDEFINITION_H_ */