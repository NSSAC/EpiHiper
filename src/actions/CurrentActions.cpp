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

#include <algorithm>

#include "actions/CurrentActions.h"
#include "utilities/Random.h"

CurrentActions::iterator::iterator(const CurrentActions::base & actions, bool begin)
  : mpBase(begin ? &actions : NULL)
  , mIt()
  , mShuffled()
  , mItShuffled(mShuffled.begin())
  , mpAction(NULL)
{
  next();
}

CurrentActions::iterator::iterator(const iterator & src)
  : mpBase(src.mpBase)
  , mIt(src.mIt)
  , mShuffled(src.mShuffled)
  , mItShuffled(mShuffled.begin() + (src.mItShuffled - src.mShuffled.begin()))
  , mpAction(src.mpAction)
{}

CurrentActions::iterator::~iterator()
{}

CurrentActions::iterator & CurrentActions::iterator::next()
{
  if (mpBase == NULL ||
      mpBase->size() == 0)
    {
      mpBase = NULL;
      mpAction = NULL;

      return *this;
    }

  if (mItShuffled != mShuffled.end())
    {
      ++mItShuffled;
      mpAction = *mItShuffled;

      return *this;
    }

  if (mpAction == NULL)
    {
      mIt = mpBase->begin();
    }
  else
    {
      ++mIt;
    }

  if (mIt == mpBase->end())
    {
      mpBase = NULL;
      mpAction = NULL;

      return *this;
    }

  mShuffled.resize(mIt->second.size());
  std::vector< Action const * >::iterator itShuffled= mShuffled.begin();
  std::vector< Action >::const_iterator it = mIt->second.begin();
  std::vector< Action >::const_iterator end = mIt->second.begin();

  for (; it != end; ++it, ++itShuffled)
    {
      *itShuffled = &*it;
    }

  std::shuffle(mShuffled.begin(), mShuffled.end(), Random::G);
  mItShuffled = mShuffled.begin();
  mpAction = *mItShuffled;

  return * this;
}

const Action * CurrentActions::iterator::operator->() const
{
  return mpAction;
}

bool CurrentActions::iterator::operator!=(const CurrentActions::iterator & rhs) const
{
  return mpAction != rhs.mpAction;
}

CurrentActions::CurrentActions()
  : CurrentActions::base()
{}

// virtual
CurrentActions::~CurrentActions()
{}

void CurrentActions::addAction(const Action & action)
{
  base::iterator found = insert(std::make_pair(action.getPriority(), std::vector< Action >())).first;
  found->second.push_back(action);
}

CurrentActions::iterator CurrentActions::begin()
{
  return iterator(*this, true);
}

CurrentActions::iterator CurrentActions::end()
{
  return iterator(*this, false);
}
