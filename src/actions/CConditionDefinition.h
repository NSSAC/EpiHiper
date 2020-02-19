// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#ifndef SRC_ACTIONS_CCONDITIONDEFINITION_H_
#define SRC_ACTIONS_CCONDITIONDEFINITION_H_

#include <vector>

#include "math/CValueList.h"
#include "math/CComputable.h"

class CObservable;
class CNodeProperty;
class CEdgeProperty ;
class CVariable;
class CSizeOf;
class CCondition;
class CNode;
class CEdge;

struct json_t;

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
    Comparison
  };

  enum struct ValueType
  {
    Value,
    ValueList,
    Observable,
    NodeProperty,
    EdgeProperty,
    Variable,
    Sizeof
  };

private:
  struct ValueInstance
  {
    size_t * pCounter;
    ValueType type;
    CValue * pValue;
    CValueList * pValueList;
    CObservable * pObservable;
    CNodeProperty * pNodeProperty;
    CEdgeProperty * pEdgeProperty;
    CVariable * pVariable;
    CSizeOf * pSizeOf;
    bool valid;

    ValueInstance();
    ValueInstance(const ValueInstance & src);
    ~ValueInstance();
    virtual void fromJSON(const json_t * json);
    CValueInterface * value(const CNode * pNode) const;
    CValueInterface * value(const CEdge * pEdge) const;
    CValueInterface * value() const;
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

  CCondition * createCondition() const;

  CCondition * createCondition(const CNode * pNode) const;

  CCondition * createCondition(const CEdge * pEdge) const;

private:
  BooleanOperationType mType;
  ComparisonType mComparison;
  ValueInstance mLeft;
  ValueInstance mRight;
  bool mValue;
  std::vector< CConditionDefinition > mBooleanValues;
  bool mValid;
};

#endif /* SRC_ACTIONS_CCONDITIONDEFINITION_H_ */
