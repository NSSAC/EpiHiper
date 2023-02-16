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

#include <algorithm>

#include "actions/CCurrentActions.h"
#include "utilities/CRandom.h"
#include "utilities/CLogger.h"

CCurrentActions::iterator::iterator(const CCurrentActions::base & actions, bool begin, bool shuffle)
  : mpBase(begin ? &actions : NULL)
  , mIt()
  , mShuffled()
  , mItShuffled(mShuffled.begin())
  , mpAction(NULL)
  , mShuffle(shuffle)
{
  next();
}

CCurrentActions::iterator::iterator(const iterator & src)
  : mpBase(src.mpBase)
  , mIt(src.mIt)
  , mShuffled(src.mShuffled)
  , mItShuffled(mShuffled.begin() + (src.mItShuffled - src.mShuffled.begin()))
  , mpAction(src.mpAction)
  , mShuffle(src.mShuffle)
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
  std::vector< CAction const * >::iterator itShuffled = mShuffled.begin();
  std::vector< CAction * >::const_iterator it = mIt->second.begin();
  std::vector< CAction * >::const_iterator end = mIt->second.end();

  for (; it != end; ++it, ++itShuffled)
    {
      *itShuffled = &(**it);
    }

  if (mShuffle)
    std::shuffle(mShuffled.begin(), mShuffled.end(), CRandom::G.Active());
  
  mItShuffled = mShuffled.begin();
  mpAction = *mItShuffled;

  return * this;
}

const CAction * CCurrentActions::iterator::operator->() const
{
  return mpAction;
}

const CAction * CCurrentActions::iterator::operator*()
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

CCurrentActions::iterator CCurrentActions::begin(bool shuffle)
{
  return iterator(*this, true, shuffle);
}

CCurrentActions::iterator CCurrentActions::end(bool shuffle)
{
  return iterator(*this, false, shuffle);
}

void CCurrentActions::clear()
{
  base::clear();
}

