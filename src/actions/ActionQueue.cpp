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

#include "ActionQueue.h"

// static
ActionQueue ActionQueue::INSTANCE;

// static
void ActionQueue::addAction(size_t deltaTick, const Action & action)
{
  INSTANCE[INSTANCE.mCurrenTick + deltaTick].addAction(action);
}

// static
bool ActionQueue::processCurrentActions()
{
  bool success = true;

  CurrentActions Actions = INSTANCE[INSTANCE.mCurrenTick];
  INSTANCE.erase(INSTANCE.mCurrenTick);

  while (true)
    {
      CurrentActions::iterator it = Actions.begin();
      CurrentActions::iterator end = Actions.end();

      for (; it != end; it.next())
        if (it->getCondition().isTrue())
          {
            std::vector< Operation >::const_iterator itOperation = it->getOperations().begin();
            std::vector< Operation >::const_iterator endOperation = it->getOperations().end();

            for (; itOperation != endOperation; ++itOperation)
              {
                success &= itOperation->execute();
              }
          }

      // TODO CRITICAL Implement me!
      // MPI Broadcast scheduled remote actions

      Actions = INSTANCE[INSTANCE.mCurrenTick];
      INSTANCE.erase(INSTANCE.mCurrenTick);

      // TODO CRITICAL Implement me!
      // MPI Broadcast Actions size

      // TODO CRITICAL Implement me!
      // if (Total actions size == 0) break;
    }

  return success;
}

// static
void ActionQueue::setCurrentTick(const size_t & currentTick)
{
  INSTANCE.mCurrenTick = currentTick;
}

// static
void ActionQueue::incrementTick()
{
  ++INSTANCE.mCurrenTick;
}

ActionQueue::ActionQueue()
  : std::map< size_t, CurrentActions >()
  , mCurrenTick(-1)
{}

// virtual
ActionQueue::~ActionQueue()
{}

