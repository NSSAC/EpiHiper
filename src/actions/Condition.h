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

#include <vector>

struct json_t;
class State;

class Boolean
{
protected:
  Boolean();

public:
  Boolean(const Boolean & src);
  virtual ~Boolean();
  virtual Boolean * copy() const = 0;

  virtual bool isTrue() const = 0;
  virtual void fromJSON(const json_t * json) = 0;
};


class Condition : protected Boolean
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

  template< typename Value > class Comparison : public Boolean
  {
  public:
    Comparison() = delete;
    Comparison(ComparisonType operation, Value & left, Value & right);
    Comparison(ComparisonType operation, Value & left, Value * right);
    Comparison(ComparisonType operation, Value * pLeft, Value & right);
    Comparison(ComparisonType operation, Value * left, Value * right);
    Comparison(const Comparison & src);
    virtual ~Comparison();
    virtual Boolean * copy() const;

    virtual bool isTrue() const;
    virtual void fromJSON(const json_t * json);

  private:
    ComparisonType mType;
    bool mOwnLeft;
    Value * mpLeft;
    bool mOwnRight;
    Value * mpRight;
  };

  class BooleanOperation : protected Boolean
  {
  public:
    BooleanOperation() = delete;
    BooleanOperation(BooleanOperationType operation, const Boolean & boolean);
    BooleanOperation(BooleanOperationType operation, const std::vector< Boolean > & booleanVector);
    virtual ~BooleanOperation();
    virtual Boolean * copy();

    virtual bool isTrue() const;
    virtual void fromJSON(const json_t * json);

  private:


  };

  template< class Value, class SetElement > class ContainedIn : protected Boolean
  {
  public:
    ContainedIn() = delete;
    ContainedIn(const Value & value, const std::vector< SetElement > & set);
    virtual ~ContainedIn();
    virtual Boolean * copy() const;

    virtual bool isTrue() const;
    virtual void fromJSON(const json_t * json);

  };

  /**
   * Default constructor
   */
  Condition() = delete;

  Condition(const Boolean & boolean);

  /**
   * Copy construnctor
   * @param const Condition & src
   */
  Condition(const Condition & src);

  /**
   * Destructor
   */
  virtual ~Condition();
  virtual Boolean * copy() const;

  bool isTrue() const;
  void fromJSON(const json_t * json);

  private:
  Boolean * mpBoolean;

};

template < class Value > Condition::Comparison< Value >::Comparison(ComparisonType operation, Value & left, Value & right)
  : Boolean()
  , mType(operation)
  , mOwnLeft(true)
  , mpLeft(new Value(left))
  , mOwnRight(true)
  , mpRight(new Value(right))
{}

template < class Value > Condition::Comparison< Value >::Comparison(ComparisonType operation, Value & left, Value * pRight)
  : Boolean()
  , mType(operation)
  , mOwnLeft(true)
  , mpLeft(new Value(left))
  , mOwnRight(false)
  , mpRight(pRight)
{}

template < class Value > Condition::Comparison< Value >::Comparison(ComparisonType operation, Value * pLeft, Value & right)
  : Boolean()
  , mType(operation)
  , mOwnLeft(false)
  , mpLeft(pLeft)
  , mOwnRight(true)
  , mpRight(new Value(right))
{}

template < class Value > Condition::Comparison< Value >::Comparison(ComparisonType operation, Value * pLeft, Value * pRight)
  : Boolean()
  , mType(operation)
  , mOwnLeft(false)
  , mpLeft(pLeft)
  , mOwnRight(false)
  , mpRight(pRight)
{}

template < class Value > Condition::Comparison< Value >::Comparison(const Comparison & src)
  : Boolean(src)
  , mType(src.mType)
  , mOwnLeft(src.mOwnLeft)
  , mpLeft(src.mOwnLeft ? new Value(*src.mpLeft): src.mpLeft)
  , mOwnRight(src.mOwnRight)
  , mpRight(src.mOwnRight ? new Value(*src.mpRight): src.mpRight)
{}

// virtual
template < class Value > Condition::Comparison< Value >::~Comparison()
{
  if (mOwnLeft) delete mpLeft;
  if (mOwnRight) delete mpRight;
}

// virtual
template < class Value > Boolean * Condition::Comparison< Value >::copy() const
{
  return new Comparison< Value >(*this);
}

// virtual
template < class Value > bool Condition::Comparison< Value >::isTrue() const
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
template < class Value > void Condition::Comparison< Value >::fromJSON(const json_t * json)
{
  // TODO CRITICAL Implement me!
}

#endif /* SRC_ACTIONS_CONDITION_H_ */
