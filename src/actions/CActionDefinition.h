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

#ifndef SRC_ACTIONS_CACTIONDEFINITION_H_
#define SRC_ACTIONS_CACTIONDEFINITION_H_

#include <vector>

#include "utilities/CAnnotation.h"
#include "utilities/CMetadata.h"
#include "actions/COperationDefinition.h"
#include "actions/CConditionDefinition.h"

class CActionDefinition : public CAnnotation
{
public:
  static void convertPrioritiesToOrder();

  static size_t OrderSize();

  CActionDefinition();

  CActionDefinition(const CActionDefinition & src);

  CActionDefinition(const json_t * json);

  virtual ~CActionDefinition();

  virtual void fromJSON(const json_t * json) override;

  const bool & isValid() const;

  void process() const;

  void process(const CEdge * pEdge) const;

  void process(const CNode * pNode) const;

  bool execute() const;

  bool execute(CEdge * pEdge) const;

  bool execute(CNode * pNode) const;

  const size_t & getDelay() const;
  
  size_t getOrder() const;

  static CActionDefinition * GetActionDefinition(const size_t & index);

private:
  static std::vector< CActionDefinition * > INSTANCES;

  static std::map< double, std::set< CActionDefinition * > > Priorities;

  std::vector< COperationDefinition > mOperations;
  double mPriority;
  size_t mOrder;
  size_t mDelay;
  CConditionDefinition mCondition;
  size_t mIndex;
  bool mValid;
  CMetadata mInfo;
};


#endif /* SRC_ACTIONS_CACTIONDEFINITION_H_ */
