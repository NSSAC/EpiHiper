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

  // We need to enter the loop at least once
  do
    {
      CurrentActions Actions = INSTANCE[INSTANCE.mCurrenTick];
      INSTANCE.erase(INSTANCE.mCurrenTick);

      CurrentActions::iterator it = Actions.begin();
      CurrentActions::iterator end = Actions.end();

      for (; it != end; it.next())
        if (it->getCondition().isTrue())
          {
            std::vector< Operation * >::const_iterator itOperation = it->getOperations().begin();
            std::vector< Operation * >::const_iterator endOperation = it->getOperations().end();

            for (; itOperation != endOperation; ++itOperation)
              {
                success &= (*itOperation)->execute(it->getMetadata());
              }
          }

      // MPI Broadcast scheduled remote actions and whether local actions are pending
      INSTANCE.broadcastRemoteActions();

      // If no local actions are pending anywhere and no remote actions where broadcasted, i.e.,
      // if (Total actions size == 0) break;
    } while (INSTANCE.mTotalPendingActions > 0);

  return success;
}

// static
size_t ActionQueue::pendingActions()
{
  return INSTANCE[INSTANCE.mCurrenTick].size();
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
  , mTotalPendingActions(0)
{}

// virtual
ActionQueue::~ActionQueue()
{}

int ActionQueue::broadcastRemoteActions()
{
  std::ostringstream os;

  mTotalPendingActions = pendingActions();
  os.write(reinterpret_cast<const char *>(&mTotalPendingActions), sizeof(size_t));

  std::string Buffer = os.str();

  Communicate::ClassMemberReceive< ActionQueue > Receive(&ActionQueue::INSTANCE, &ActionQueue::receiveActions);

  // std::cout << Communicate::Rank << ": ActionQueue::broadcastRemoteActions" << std::endl;
  Communicate::broadcast(Buffer.c_str(), Buffer.length(), &Receive);

  return (int) Communicate::ErrorCode::Success;
}

Communicate::ErrorCode ActionQueue::receiveActions(std::istream & is, int sender)
{
  size_t RemotePendingActions;

  is.read(reinterpret_cast<char *>(&RemotePendingActions), sizeof(size_t));
  mTotalPendingActions += RemotePendingActions;

  return Communicate::ErrorCode::Success;
}


