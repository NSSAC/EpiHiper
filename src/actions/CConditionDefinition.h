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

#ifndef SRC_ACTIONS_CCONDITIONDEFINITION_H_
#define SRC_ACTIONS_CCONDITIONDEFINITION_H_

#include <memory>
#include <vector>

#include "math/CValueList.h"
#include "math/CComputable.h"
#include "math/CValueInstance.h"

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
    GreaterOrEqual,
    Within,
    NotWithin
  };

  enum struct BooleanOperationType
  {
    And,
    Or,
    Not,
    Value,
    Comparison,
    __SIZE
  };

public:
  static CComputableSet RequiredComputables;

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

  virtual void fromJSON(const json_t * json);

  bool valueFromJSON(const json_t * json);

  bool comparisonFromJSON(const json_t * json);

  bool operationFromJSON(const json_t * json);

  const bool & isValid() const;

  bool isTrue() const;

  bool isTrue(const CNode * pNode) const;

  bool isTrue(const CEdge * pEdge) const;

private:
  BooleanOperationType mType;
  ComparisonType mComparison;
  CValueInstance mLeft;
  CValueInstance mRight;
  bool mValue;
  std::vector< CConditionDefinition > mBooleanValues;
  bool mValid;
};

#endif /* SRC_ACTIONS_CCONDITIONDEFINITION_H_ */
