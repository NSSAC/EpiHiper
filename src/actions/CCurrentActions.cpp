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

#include <algorithm>

#include "actions/CCurrentActions.h"
#include "utilities/CRandom.h"

CCurrentActions::iterator::iterator(const CCurrentActions::base & actions, bool begin)
  : mpBase(begin ? &actions : NULL)
  , mIt()
  , mShuffled()
  , mItShuffled(mShuffled.begin())
  , mpAction(NULL)
{
  next();
}

CCurrentActions::iterator::iterator(const iterator & src)
  : mpBase(src.mpBase)
  , mIt(src.mIt)
  , mShuffled(src.mShuffled)
  , mItShuffled(mShuffled.begin() + (src.mItShuffled - src.mShuffled.begin()))
  , mpAction(src.mpAction)
{}

CCurrentActions::iterator::~iterator()
{}

CCurrentActions::iterator & CCurrentActions::iterator::next()
{
  if (mpBase == NULL ||
      mpBase->size() == 0)
    {
      mpBase = NULL;
      mpAction = NULL;

      return *this;
    }

  if (mpAction == NULL)
    {
      mIt = mpBase->begin();
    }
  else
    {
      ++mItShuffled;

      if (mItShuffled != mShuffled.end())
        {
          mpAction = *mItShuffled;

          return *this;
        }

      ++mIt;
    }

  if (mIt == mpBase->end())
    {
      mpBase = NULL;
      mpAction = NULL;

      return *this;
    }

  mShuffled.resize(mIt->second.size());
  std::vector< CAction const * >::iterator itShuffled= mShuffled.begin();
  std::vector< CAction * >::const_iterator it = mIt->second.begin();
  std::vector< CAction * >::const_iterator end = mIt->second.end();

  for (; it != end; ++it, ++itShuffled)
    {
      *itShuffled = &(**it);
    }

  std::shuffle(mShuffled.begin(), mShuffled.end(), CRandom::G);
  mItShuffled = mShuffled.begin();
  mpAction = *mItShuffled;

  return * this;
}

const CAction * CCurrentActions::iterator::operator->() const
{
  return mpAction;
}

bool CCurrentActions::iterator::operator!=(const CCurrentActions::iterator & rhs) const
{
  return mpAction != rhs.mpAction;
}

CCurrentActions::CCurrentActions()
  : CCurrentActions::base()
{}

// virtual
CCurrentActions::~CCurrentActions()
{
  base::iterator itMap = base::begin();
  base::iterator endMap = base::end();

  for (; itMap != endMap; ++itMap)
    {
      std::vector< CAction * >::iterator it = itMap->second.begin();
      std::vector< CAction * >::iterator end = itMap->second.end();

      for (; it != end; ++it)
        delete *it;
    }
}

void CCurrentActions::addAction(CAction * pAction)
{
  base::iterator found = insert(std::make_pair(pAction->getPriority(), std::vector< CAction * >())).first;
  found->second.push_back(pAction);
}

size_t CCurrentActions::size() const
{
  return base::size();
}

CCurrentActions::iterator CCurrentActions::begin()
{
  return iterator(*this, true);
}

CCurrentActions::iterator CCurrentActions::end()
{
  return iterator(*this, false);
}
