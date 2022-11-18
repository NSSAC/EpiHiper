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
class CObservable;
class CSizeOf;
class CNode;
class CEdge;
class CMetadata;
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

  virtual void fromJSON(const json_t * json) override;

  const bool & isValid() const;

  COperation * createOperation(CNode * pNode, const CMetadata & info) const;

  COperation * createOperation(CEdge * pEdge, const CMetadata & info) const;

  COperation * createOperation(const CMetadata & info) const;

private:
  TargetType mTargetType;
  CNodeProperty * mpNodeProperty;
  CEdgeProperty * mpEdgeProperty;
  CVariable * mpTargetVariable;
  CValueInterface::pOperator mpOperator;
  CVariable * mpSourceVariable;
  CObservable * mpObservable;
  CSizeOf * mpSizeOf;
  CValue mValue;
  bool mValid;
};
#endif /* SRC_ACTIONS_COPERATIONDEFINITION_H_ */
