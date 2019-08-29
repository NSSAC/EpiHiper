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
  virtual void fromJSON(const json_t * json) = 0;
};


class CCondition : protected CBoolean
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

  template< typename Value > class CComparison : public CBoolean
  {
  public:
    CComparison() = delete;
    CComparison(ComparisonType operation, Value const & left, Value const & right);
    CComparison(ComparisonType operation, Value const & left, Value const * right);
    CComparison(ComparisonType operation, Value const * pLeft, Value const & right);
    CComparison(ComparisonType operation, Value const * left, Value const * right);
    CComparison(const CComparison & src);
    virtual ~CComparison();
    virtual CBoolean * copy() const;

    virtual bool isTrue() const;
    virtual void fromJSON(const json_t * json);

  private:
    ComparisonType mType;
    bool mOwnLeft;
    Value const * mpLeft;
    bool mOwnRight;
    Value const * mpRight;
  };

  class CBooleanOperation : protected CBoolean
  {
  public:
    CBooleanOperation() = delete;
    CBooleanOperation(BooleanOperationType operation, const CBoolean & boolean);
    CBooleanOperation(BooleanOperationType operation, const std::vector< CBoolean > & booleanVector);
    virtual ~CBooleanOperation();
    virtual CBoolean * copy();

    virtual bool isTrue() const;
    virtual void fromJSON(const json_t * json);

  private:


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
  void fromJSON(const json_t * json);

  private:
  CBoolean * mpBoolean;

};

template < class Value > CCondition::CComparison< Value >::CComparison(ComparisonType operation, Value const & left, Value const & right)
  : CBoolean()
  , mType(operation)
  , mOwnLeft(true)
  , mpLeft(new Value(left))
  , mOwnRight(true)
  , mpRight(new Value(right))
{}

template < class Value > CCondition::CComparison< Value >::CComparison(ComparisonType operation, Value const & left, Value const * pRight)
  : CBoolean()
  , mType(operation)
  , mOwnLeft(true)
  , mpLeft(new Value(left))
  , mOwnRight(false)
  , mpRight(pRight)
{}

template < class Value > CCondition::CComparison< Value >::CComparison(ComparisonType operation, Value const * pLeft, Value const & right)
  : CBoolean()
  , mType(operation)
  , mOwnLeft(false)
  , mpLeft(pLeft)
  , mOwnRight(true)
  , mpRight(new Value(right))
{}

template < class Value > CCondition::CComparison< Value >::CComparison(ComparisonType operation, Value const * pLeft, Value const * pRight)
  : CBoolean()
  , mType(operation)
  , mOwnLeft(false)
  , mpLeft(pLeft)
  , mOwnRight(false)
  , mpRight(pRight)
{}

template < class Value > CCondition::CComparison< Value >::CComparison(const CComparison & src)
  : CBoolean(src)
  , mType(src.mType)
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
  switch (mType)
  {
    case ComparisonType::Equal:
      return (*mpLeft == *mpRight);
      break;

    case ComparisonType::NotEqual:
      return (*mpLeft != *mpRight);
      break;

    case ComparisonType::Less:
      return (*mpLeft < *mpRight);
      break;

    case ComparisonType::LessOrEqual:
      return (*mpLeft <= *mpRight);
      break;

    case ComparisonType::Greater:
      return (*mpLeft > *mpRight);
      break;

    case ComparisonType::GreaterOrEqual:
      return (*mpLeft >= *mpRight);
      break;
  }

  return false;
}

// virtual
template < class Value > void CCondition::CComparison< Value >::fromJSON(const json_t * json)
{
  // TODO CRITICAL Implement me!
}

#endif /* SRC_ACTIONS_CCONDITION_H_ */
