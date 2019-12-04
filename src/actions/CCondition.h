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

#ifndef SRC_ACTIONS_CCONDITION_H_
#define SRC_ACTIONS_CCONDITION_H_

#include <vector>

#include "actions/CConditionDefinition.h"
#include "math/CComputable.h"

struct json_t;
class CHealthState;

class CBoolean
{
protected:
  CBoolean();

public:
  CBoolean(const CBoolean & src);
  virtual ~CBoolean();
  virtual CBoolean * copy() const = 0;

  virtual bool isTrue() const = 0;
};


class CCondition : protected CBoolean
{
public:
  class CComparison : public CBoolean
  {
  public:
    typedef bool (*pComparison)(const CValueInterface &, const CValueInterface &);

    CComparison() = delete;
    CComparison(CConditionDefinition::ComparisonType operation, CValueInterface const & left, CValueInterface const & right);
    CComparison(CConditionDefinition::ComparisonType operation, CValueInterface const & left, CValueInterface const * right);
    CComparison(CConditionDefinition::ComparisonType operation, CValueInterface const * left, CValueInterface const & right);
    CComparison(CConditionDefinition::ComparisonType operation, CValueInterface const * left, CValueInterface const * right);

    CComparison(const CComparison & src);
    virtual ~CComparison();
    virtual CBoolean * copy() const;

    virtual bool isTrue() const;

  private:
    void selectComparison(CConditionDefinition::ComparisonType operation);

    pComparison mpComparison;
    bool mOwnLeft;
    CValueInterface const * mpLeft;
    bool mOwnRight;
    CValueInterface const * mpRight;
  };

  class CBooleanValue : public CBoolean
  {
  public:
    CBooleanValue() = delete;
    CBooleanValue(const bool & value);
    CBooleanValue(const CBooleanValue & src);
    virtual ~CBooleanValue();
    virtual CBoolean * copy() const;

    virtual bool isTrue() const;

  private:
    bool mTrue;
  };

  class CBooleanOperation : public CBoolean
  {
  private:
    bool _and() const;
    bool _or() const;
    bool _not() const;

  public:
    typedef bool (CBooleanOperation::*pOperation)() const;

    CBooleanOperation() = delete;
    CBooleanOperation(CConditionDefinition::BooleanOperationType operation, const std::vector< CBoolean * > & booleanVector);
    CBooleanOperation(const CBooleanOperation & src);
    virtual ~CBooleanOperation();
    virtual CBoolean * copy() const;

    virtual bool isTrue() const;
  private:
    pOperation mpOperation;
    std::vector< CBoolean * > mVector;
  };

  class CContainedIn : public CBoolean
  {
  private:
    static bool within(const CValueInterface &, const CValueList &);
    static bool notWithin(const CValueInterface &, const CValueList &);

  public:
    typedef bool (*pWithin)(const CValueInterface &, const CValueList &);

    CContainedIn() = delete;
    CContainedIn(CConditionDefinition::ComparisonType operation, const CValueInterface & value, const CValueList & set);
    CContainedIn(CConditionDefinition::ComparisonType operation, const CValueInterface * pValue, const CValueList & set);
    CContainedIn(const CContainedIn & src);
    virtual ~CContainedIn();
    virtual CBoolean * copy() const;

    virtual bool isTrue() const;

  private:
    pWithin mpWithin;
    bool mOwnLeft;
    CValueInterface const * mpLeft;
    const CValueList & mValueList;
  };

  /**
   * Default constructor
   */
  CCondition() = delete;

  CCondition(const CBoolean & boolean);

  /**
   * Copy construnctor
   * @param const Condition & src
   */
  CCondition(const CCondition & src);

  /**
   * Destructor
   */
  virtual ~CCondition();
  virtual CBoolean * copy() const;

  bool isTrue() const;

  private:
  CBoolean * mpBoolean;

};



#endif /* SRC_ACTIONS_CCONDITION_H_ */
