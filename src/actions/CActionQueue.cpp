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

#include "actions/CActionQueue.h"
#include "actions/CActionDefinition.h"

#include "network/CNetwork.h"
#include "network/CNode.h"
#include "network/CEdge.h"

// static
CActionQueue CActionQueue::INSTANCE;

// static
void CActionQueue::addAction(size_t deltaTick, const CAction & action)
{
  INSTANCE[INSTANCE.mCurrenTick + deltaTick].addAction(action);
}

// static
bool CActionQueue::processCurrentActions()
{
  bool success = true;

  // We need to enter the loop at least once
  do
    {
      CCurrentActions Actions = INSTANCE[INSTANCE.mCurrenTick];
      INSTANCE.erase(INSTANCE.mCurrenTick);

      CCurrentActions::iterator it = Actions.begin();
      CCurrentActions::iterator end = Actions.end();

      for (; it != end; it.next())
        if (it->getCondition().isTrue())
          {
            std::vector< COperation * >::const_iterator itOperation = it->getOperations().begin();
            std::vector< COperation * >::const_iterator endOperation = it->getOperations().end();

            for (; itOperation != endOperation; ++itOperation)
              {
                success &= (*itOperation)->execute(it->getMetadata());
              }
          }

      // MPI Broadcast scheduled remote actions and whether local actions are pending
      INSTANCE.broadcastPendingActions();

      // If no local actions are pending anywhere and no remote actions where broadcasted, i.e.,
      // if (Total actions size == 0) break;
    } while (INSTANCE.mTotalPendingActions > 0);

  return success;
}

// static
size_t CActionQueue::pendingActions()
{
  return INSTANCE[INSTANCE.mCurrenTick].size();
}

// static
const int & CActionQueue::getCurrentTick()
{
  return INSTANCE.mCurrenTick;
}


// static
void CActionQueue::setCurrentTick(const int & currentTick)
{
  INSTANCE.mCurrenTick = currentTick;
}

// static
void CActionQueue::incrementTick()
{
  ++INSTANCE.mCurrenTick;
}

// static
void CActionQueue::addRemoteAction(const size_t & index, const CNode * pNode)
{
  INSTANCE.mRemoteActions.write(reinterpret_cast<const char *>(&index), sizeof(size_t));
  INSTANCE.mRemoteActions << 'N';
  INSTANCE.mRemoteActions.write(reinterpret_cast<const char *>(&pNode->id), sizeof(size_t));
}

// static
void CActionQueue::addRemoteAction(const size_t & index, const CEdge * pEdge)
{
  INSTANCE.mRemoteActions.write(reinterpret_cast<const char *>(&index), sizeof(size_t));
  INSTANCE.mRemoteActions << 'E';
  INSTANCE.mRemoteActions.write(reinterpret_cast<const char *>(&pEdge->targetId), sizeof(size_t));
  INSTANCE.mRemoteActions.write(reinterpret_cast<const char *>(&pEdge->sourceId), sizeof(size_t));
}


CActionQueue::CActionQueue()
  : std::map< int, CCurrentActions >()
  , mCurrenTick(-1)
  , mTotalPendingActions(0)
{}

// virtual
CActionQueue::~CActionQueue()
{}

int CActionQueue::broadcastPendingActions()
{
  std::ostringstream os;

  mTotalPendingActions = pendingActions();
  os.write(reinterpret_cast<const char *>(&mTotalPendingActions), sizeof(size_t));
  os << mRemoteActions.str();

  std::string Buffer = os.str();

  CCommunicate::ClassMemberReceive< CActionQueue > Receive(&CActionQueue::INSTANCE, &CActionQueue::receivePendingActions);

  // std::cout << Communicate::Rank << ": ActionQueue::broadcastRemoteActions" << std::endl;
  CCommunicate::broadcast(Buffer.c_str(), Buffer.length(), &Receive);

  return (int) CCommunicate::ErrorCode::Success;
}

CCommunicate::ErrorCode CActionQueue::receivePendingActions(std::istream & is, int sender)
{
  size_t RemotePendingActions;

  is.read(reinterpret_cast<char *>(&RemotePendingActions), sizeof(size_t));
  mTotalPendingActions += RemotePendingActions;

  // Check whether we received actions from remote;
  while(is.good())
    {
      size_t ActionId;
      CActionDefinition * pActionDefinition = CActionDefinition::GetActionDefinition(ActionId);
      char TargetType;
      is.read(reinterpret_cast<char *>(&ActionId), sizeof(size_t));
      is.read(&TargetType, sizeof(char));

      switch (TargetType)
      {
        case 'N':
          {
            size_t NodeId;
            is.read(reinterpret_cast<char *>(&NodeId), sizeof(size_t));
            CNode * pNode = CNetwork::INSTANCE->lookupNode(NodeId, true);

            if (pNode != NULL &&
                pActionDefinition != NULL)
              {
                pActionDefinition->process(pNode);
                ++mTotalPendingActions;
              }
          }
          break;

        case 'E':
          {
            size_t TargetId;
            is.read(reinterpret_cast<char *>(&TargetId), sizeof(size_t));
            size_t SourceId;
            is.read(reinterpret_cast<char *>(&SourceId), sizeof(size_t));
            CEdge * pEdge = CNetwork::INSTANCE->lookupEdge(TargetId, SourceId);

            if (pEdge != NULL &&
                pActionDefinition != NULL)
              {
                pActionDefinition->process(pEdge);
              }
          }
          break;
      }
    }

  return CCommunicate::ErrorCode::Success;
}


