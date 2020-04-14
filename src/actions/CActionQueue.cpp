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

#include "actions/CActionQueue.h"
#include "actions/CActionDefinition.h"

#include "network/CNetwork.h"
#include "network/CNode.h"
#include "network/CEdge.h"
#include "utilities/CLogger.h"

// static
void CActionQueue::init()
{
  if (pINSTANCE == NULL)
    {
      pINSTANCE = new CActionQueue();
    }
}

// static
void CActionQueue::release()
{
  if (pINSTANCE != NULL)
    {
      delete pINSTANCE;
      pINSTANCE = NULL;
    }
}

// static
void CActionQueue::addAction(size_t deltaTick, CAction * pAction)
{
  if (pINSTANCE != NULL)
    {
      try
        {
          base::iterator found = pINSTANCE->find(pINSTANCE->mCurrenTick + deltaTick);

          if (found == pINSTANCE->end())
            {
              found = pINSTANCE->insert(std::make_pair(pINSTANCE->mCurrenTick + deltaTick, new CCurrentActions())).first;
            }

          found->second->addAction(pAction);
        }
      catch (...)
        {
          CLogger::error("CActionQueue: Failed to add action.");
        }
    }
}

// static
bool CActionQueue::processCurrentActions()
{
  if (pINSTANCE == NULL)
    return false;

  bool success = true;

  // We need to enter the loop at least once
  do
    {
      CCurrentActions * pActions = NULL;
      base::iterator found = pINSTANCE->find(pINSTANCE->mCurrenTick);

      if (found != pINSTANCE->end())
        {
          pActions = found->second;
        }
      else
        {
          pActions = new CCurrentActions();
        }

      pINSTANCE->erase(pINSTANCE->mCurrenTick);

      CCurrentActions::iterator it = pActions->begin();
      CCurrentActions::iterator end = pActions->end();

      for (; it != end; it.next())
        success &= it->execute();

      delete pActions;

      // MPI Broadcast scheduled remote actions and whether local actions are pending
      CCommunicate::barrierRMA();
      pINSTANCE->broadcastPendingActions();

      // If no local actions are pending anywhere and no remote actions where broadcasted, i.e.,
      // if (Total actions size == 0) break;
    }
  while (pINSTANCE->mTotalPendingActions > 0);

  return success;
}

// static
size_t CActionQueue::pendingActions()
{
  if (pINSTANCE != NULL)
    {
      base::iterator found = pINSTANCE->find(pINSTANCE->mCurrenTick);

      if (found == pINSTANCE->end())
        {
          return 0;
        }

      return found->second->size();
    }

  return 0;
}

// static
const CTick & CActionQueue::getCurrentTick()
{
  if (pINSTANCE != NULL)
    return pINSTANCE->mCurrenTick;

  static CTick Zero(0);

  return Zero;
}

// static
void CActionQueue::setCurrentTick(const int & currentTick)
{
  if (pINSTANCE != NULL)
    pINSTANCE->mCurrenTick = currentTick;
}

// static
void CActionQueue::incrementTick()
{
  if (pINSTANCE != NULL)
    ++pINSTANCE->mCurrenTick;
}

// static
void CActionQueue::addRemoteAction(const size_t & index, const CNode * pNode)
{
  if (pINSTANCE != NULL)
    {
      try
        {
          pINSTANCE->mRemoteActions.write(reinterpret_cast< const char * >(&index), sizeof(size_t));
          pINSTANCE->mRemoteActions << 'N';
          pINSTANCE->mRemoteActions.write(reinterpret_cast< const char * >(&pNode->id), sizeof(size_t));
        }
      catch (...)
        {
          CLogger::error() << "CActionQueue: Add remote action failed for action '" << index << "' node '" << pNode->id << "'.";
        }
    }
}

// static
void CActionQueue::addRemoteAction(const size_t & index, const CEdge * pEdge)
{
  if (pINSTANCE != NULL)
    {
      try
        {
          pINSTANCE->mRemoteActions.write(reinterpret_cast< const char * >(&index), sizeof(size_t));
          pINSTANCE->mRemoteActions << 'E';
          pINSTANCE->mRemoteActions.write(reinterpret_cast< const char * >(&pEdge->targetId), sizeof(size_t));
          pINSTANCE->mRemoteActions.write(reinterpret_cast< const char * >(&pEdge->sourceId), sizeof(size_t));
        }
      catch (...)
        {
          CLogger::error() << "CActionQueue: Add remote action failed for action '" << index << "' edge '" << pEdge->targetId << "', '" << pEdge->sourceId << "'.";
        }
    }
}

CActionQueue::CActionQueue()
  : base()
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
  os.write(reinterpret_cast< const char * >(&mTotalPendingActions), sizeof(size_t));
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

  is.read(reinterpret_cast< char * >(&RemotePendingActions), sizeof(size_t));
  mTotalPendingActions += RemotePendingActions;

  // Check whether we received actions from remote;
  while (true)
    {
      size_t ActionId;
      is.read(reinterpret_cast< char * >(&ActionId), sizeof(size_t));

      if (is.fail())
        break;

      CActionDefinition * pActionDefinition = CActionDefinition::GetActionDefinition(ActionId);

      char TargetType;
      is.read(&TargetType, sizeof(char));

      if (is.fail())
        break;

      switch (TargetType)
        {
        case 'N':
          {
            size_t NodeId;
            is.read(reinterpret_cast< char * >(&NodeId), sizeof(size_t));
            if (is.fail())
              break;

            CNode * pNode = CNetwork::INSTANCE->lookupNode(NodeId, true);

            if (pNode != NULL
                && pActionDefinition != NULL)
              {
                pActionDefinition->process(pNode);
                ++mTotalPendingActions;
              }
          }
          break;

        case 'E':
          {
            size_t TargetId;
            is.read(reinterpret_cast< char * >(&TargetId), sizeof(size_t));
            if (is.fail())
              break;

            size_t SourceId;
            is.read(reinterpret_cast< char * >(&SourceId), sizeof(size_t));
            if (is.fail())
              break;

            CEdge * pEdge = CNetwork::INSTANCE->lookupEdge(TargetId, SourceId);

            if (pEdge != NULL
                && pActionDefinition != NULL)
              {
                pActionDefinition->process(pEdge);
              }
          }
          break;
        }
    }

  return CCommunicate::ErrorCode::Success;
}
