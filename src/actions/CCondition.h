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
  template< typename Value > class CComparison : public CBoolean
  {
  private:
    static bool equal(const Value &, const Value &);
    static bool notEqual(const Value &, const Value &);
    static bool less(const Value &, const Value &);
    static bool lessOrEqual(const Value &, const Value &);
    static bool greater(const Value &, const Value &);
    static bool greaterOrEqual(const Value &, const Value &);

  public:
    typedef bool (*pComparison)(const Value &, const Value &);

    CComparison() = delete;
    CComparison(CConditionDefinition::ComparisonType operation, Value const & left, Value const & right);
    CComparison(CConditionDefinition::ComparisonType operation, Value const & left, Value const * right);
    CComparison(CConditionDefinition::ComparisonType operation, Value const * pLeft, Value const & right);
    CComparison(CConditionDefinition::ComparisonType operation, Value const * left, Value const * right);
    CComparison(const CComparison & src);
    virtual ~CComparison();
    virtual CBoolean * copy() const;

    virtual bool isTrue() const;

  private:
    void selectComparison(CConditionDefinition::ComparisonType operation);

    pComparison mpComparison;
    bool mOwnLeft;
    Value const * mpLeft;
    bool mOwnRight;
    Value const * mpRight;
  };

  class CBooleanOperation : protected CBoolean
  {
  private:
    static bool _and(const bool &, const bool &);
    static bool _or(const bool &, const bool &);
    static bool _not(const bool &, const bool &);

  public:
    typedef bool (*pOperation)(const bool &, const bool &);

    CBooleanOperation() = delete;
    CBooleanOperation(CConditionDefinition::BooleanOperationType operation, const CBoolean & boolean);
    CBooleanOperation(CConditionDefinition::BooleanOperationType operation, const std::vector< CBoolean > & booleanVector);
    virtual ~CBooleanOperation();
    virtual CBoolean * copy();

    virtual bool isTrue() const;

    // TODO CRTITICAL Implement me!
  };

  template< class Value, class SetElement > class CContainedIn : protected CBoolean
  {
  public:
    CContainedIn() = delete;
    CContainedIn(const Value & value, const std::vector< SetElement > & set);
    virtual ~CContainedIn();
    virtual CBoolean * copy() const;

    virtual bool isTrue() const;
    virtual void fromJSON(const json_t * json);

    // TODO CRTITICAL Implement me!
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

// static
template < class Value > bool CCondition::CComparison< Value >::equal(const Value & lhs, const Value & rhs)
{
  return lhs == rhs;
}

// static
template < class Value > bool CCondition::CComparison< Value >::notEqual(const Value & lhs, const Value & rhs)
{
  return lhs != rhs;
}

// static
template < class Value > bool CCondition::CComparison< Value >::less(const Value & lhs, const Value & rhs)
{
  return lhs < rhs;
}

// static
template < class Value > bool CCondition::CComparison< Value >::lessOrEqual(const Value & lhs, const Value & rhs)
{
  return lhs <= rhs;
}

// static
template < class Value > bool CCondition::CComparison< Value >::greater(const Value & lhs, const Value & rhs)
{
  return lhs > rhs;
}

// static
template < class Value > bool CCondition::CComparison< Value >::greaterOrEqual(const Value & lhs, const Value & rhs)
{
  return lhs >= rhs;
}

template < class Value > CCondition::CComparison< Value >::CComparison(CConditionDefinition::ComparisonType operation, Value const & left, Value const & right)
  : CBoolean()
  , mpComparison(NULL)
  , mOwnLeft(true)
  , mpLeft(new Value(left))
  , mOwnRight(true)
  , mpRight(new Value(right))
{
  selectComparison(operation);
}

template < class Value > CCondition::CComparison< Value >::CComparison(CConditionDefinition::ComparisonType operation, Value const & left, Value const * pRight)
  : CBoolean()
  , mpComparison(NULL)
  , mOwnLeft(true)
  , mpLeft(new Value(left))
  , mOwnRight(false)
  , mpRight(pRight)
{
  selectComparison(operation);
}

template < class Value > CCondition::CComparison< Value >::CComparison(CConditionDefinition::ComparisonType operation, Value const * pLeft, Value const & right)
  : CBoolean()
  , mpComparison(NULL)
  , mOwnLeft(false)
  , mpLeft(pLeft)
  , mOwnRight(true)
  , mpRight(new Value(right))
{
  selectComparison(operation);
}

template < class Value > CCondition::CComparison< Value >::CComparison(CConditionDefinition::ComparisonType operation, Value const * pLeft, Value const * pRight)
  : CBoolean()
  , mpComparison(NULL)
  , mOwnLeft(false)
  , mpLeft(pLeft)
  , mOwnRight(false)
  , mpRight(pRight)
{
  selectComparison(operation);
}

template < class Value > CCondition::CComparison< Value >::CComparison(const CComparison & src)
  : CBoolean(src)
  , mpComparison(src.mpComparison)
  , mOwnLeft(src.mOwnLeft)
  , mpLeft(src.mOwnLeft ? new Value(*src.mpLeft): src.mpLeft)
  , mOwnRight(src.mOwnRight)
  , mpRight(src.mOwnRight ? new Value(*src.mpRight): src.mpRight)
{}

// virtual
template < class Value > CCondition::CComparison< Value >::~CComparison()
{
  if (mOwnLeft) delete mpLeft;
  if (mOwnRight) delete mpRight;
}

// virtual
template < class Value > CBoolean * CCondition::CComparison< Value >::copy() const
{
  return new CComparison< Value >(*this);
}

// virtual
template < class Value > bool CCondition::CComparison< Value >::isTrue() const
{
  return (*mpComparison)(*mpLeft, *mpRight);
}

template < class Value > void CCondition::CComparison< Value >::selectComparison(CConditionDefinition::ComparisonType operation)
{
  switch (operation)
  {
    case CConditionDefinition::ComparisonType::Equal:
      mpComparison = &equal;
      break;

    case CConditionDefinition::ComparisonType::NotEqual:
      mpComparison = &notEqual;
      break;

    case CConditionDefinition::ComparisonType::Less:
      mpComparison = &less;
      break;

    case CConditionDefinition::ComparisonType::LessOrEqual:
      mpComparison = &lessOrEqual;
      break;

    case CConditionDefinition::ComparisonType::Greater:
      mpComparison = &greater;
      break;

    case CConditionDefinition::ComparisonType::GreaterOrEqual:
      mpComparison = &greaterOrEqual;
      break;
  }
}

#endif /* SRC_ACTIONS_CCONDITION_H_ */
