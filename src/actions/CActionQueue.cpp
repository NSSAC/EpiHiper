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
void CActionQueue::init()
{
  if (pINSTANCE == NULL)
    {
      pINSTANCE = new CActionQueue();
    }
}


// static
void CActionQueue::addAction(size_t deltaTick, const CAction & action)
{
  pINSTANCE->operator [](pINSTANCE->mCurrenTick + deltaTick).addAction(action);
}

// static
bool CActionQueue::processCurrentActions()
{
  bool success = true;

  // We need to enter the loop at least once
  do
    {
      CCurrentActions Actions = pINSTANCE->operator [](pINSTANCE->mCurrenTick);
      pINSTANCE->erase(pINSTANCE->mCurrenTick);

      CCurrentActions::iterator it = Actions.begin();
      CCurrentActions::iterator end = Actions.end();

      for (; it != end; it.next())
        if (it->getCondition().isTrue())
          {
            std::vector< COperation * >::const_iterator itOperation = it->getOperations().begin();
            std::vector< COperation * >::const_iterator endOperation = it->getOperations().end();

            for (; itOperation != endOperation; ++itOperation)
              {
                success &= (*itOperation)->execute();
              }
          }

      // MPI Broadcast scheduled remote actions and whether local actions are pending
      pINSTANCE->broadcastPendingActions();

      // If no local actions are pending anywhere and no remote actions where broadcasted, i.e.,
      // if (Total actions size == 0) break;
    } while (pINSTANCE->mTotalPendingActions > 0);

  return success;
}

// static
size_t CActionQueue::pendingActions()
{
  return pINSTANCE->operator [](pINSTANCE->mCurrenTick).size();
}

// static
const CTick & CActionQueue::getCurrentTick()
{
  return pINSTANCE->mCurrenTick;
}


// static
void CActionQueue::setCurrentTick(const int & currentTick)
{
  pINSTANCE->mCurrenTick = currentTick;
}

// static
void CActionQueue::incrementTick()
{
  ++pINSTANCE->mCurrenTick;
}

// static
void CActionQueue::addRemoteAction(const size_t & index, const CNode * pNode)
{
  pINSTANCE->mRemoteActions.write(reinterpret_cast<const char *>(&index), sizeof(size_t));
  pINSTANCE->mRemoteActions << 'N';
  pINSTANCE->mRemoteActions.write(reinterpret_cast<const char *>(&pNode->id), sizeof(size_t));
}

// static
void CActionQueue::addRemoteAction(const size_t & index, const CEdge * pEdge)
{
  pINSTANCE->mRemoteActions.write(reinterpret_cast<const char *>(&index), sizeof(size_t));
  pINSTANCE->mRemoteActions << 'E';
  pINSTANCE->mRemoteActions.write(reinterpret_cast<const char *>(&pEdge->targetId), sizeof(size_t));
  pINSTANCE->mRemoteActions.write(reinterpret_cast<const char *>(&pEdge->sourceId), sizeof(size_t));
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
  mRemoteActions.str("");

  std::string Buffer = os.str();

  CCommunicate::ClassMemberReceive< CActionQueue > Receive(CActionQueue::pINSTANCE, &CActionQueue::receivePendingActions);

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
  while(true)
    {
      size_t ActionId;
      is.read(reinterpret_cast<char *>(&ActionId), sizeof(size_t));
      if (is.fail()) break;

      CActionDefinition * pActionDefinition = CActionDefinition::GetActionDefinition(ActionId);

      char TargetType;
      is.read(&TargetType, sizeof(char));
      if (is.fail()) break;

      switch (TargetType)
      {
        case 'N':
          {
            size_t NodeId;
            is.read(reinterpret_cast<char *>(&NodeId), sizeof(size_t));
            if (is.fail()) break;

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
            if (is.fail()) break;

            size_t SourceId;
            is.read(reinterpret_cast<char *>(&SourceId), sizeof(size_t));
            if (is.fail()) break;

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


