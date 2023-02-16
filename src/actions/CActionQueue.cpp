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

#include "actions/CActionQueue.h"
#include "actions/CActionDefinition.h"
#include "actions/CNodeAction.h"
#include "actions/CEdgeAction.h"
#include "network/CNetwork.h"
#include "network/CNode.h"
#include "network/CEdge.h"
#include "utilities/CLogger.h"

// static
void CActionQueue::init()
{
  Context.init();
}

// static
void CActionQueue::release()
{
  if (Context.size() > 1)
    {
      map::iterator itAction = Context.Master().actionQueue.begin();
      map::iterator endAction = Context.Master().actionQueue.end();

      for (; itAction != endAction; ++itAction)
        {
          delete itAction->second;
        }

      map::iterator itLocal = Context.Master().locallyAdded.begin();
      map::iterator endLocal = Context.Master().locallyAdded.end();

      for (; itLocal != endLocal; ++itLocal)
        {
          delete itLocal->second;
        }
    }

  sActionQueue * pActionQueue = Context.beginThread();
  sActionQueue * pEndActionQueue = Context.endThread();

  for (; pActionQueue != pEndActionQueue; ++pActionQueue)
    {
      map::iterator itAction = pActionQueue->actionQueue.begin();
      map::iterator endAction = pActionQueue->actionQueue.end();

      for (; itAction != endAction; ++itAction)
        {
          delete itAction->second;
        }

      map::iterator itLocal = pActionQueue->locallyAdded.begin();
      map::iterator endLocal = pActionQueue->locallyAdded.end();

      for (; itLocal != endLocal; ++itLocal)
        {
          delete itLocal->second;
        }
    }
    
  Context.release();
}

// static
void CActionQueue::addAction(size_t deltaTick, CAction * pAction)
{
  try
    {
      addAction(Context.Active().actionQueue, deltaTick, pAction);
    }
  catch (...)
    {
      CLogger::error("CActionQueue: Failed to add action.");
    }
}

// static 
void CActionQueue::addAction(CActionQueue::map & queue, size_t deltaTick, CAction * pAction)
{
  map::iterator found = queue.find(CurrenTick + deltaTick);

  if (found == queue.end())
    {
      found = queue.insert(std::make_pair(CurrenTick + deltaTick, new CCurrentActions())).first;
    }

  found->second->addAction(pAction);
}

// static
bool CActionQueue::processCurrentActions()
{
  sActionQueue & ActionQueue = Context.Active();

  bool success = true;

  // We need to enter the loop at least once
  do
    {
      CCurrentActions * pActions = NULL;
      map::iterator found = ActionQueue.actionQueue.find(CurrenTick);

      if (found != ActionQueue.actionQueue.end())
        {
          pActions = found->second;
        }
      else
        {
          pActions = new CCurrentActions();
        }

      ActionQueue.actionQueue.erase(CurrenTick);

      CCurrentActions::iterator it = pActions->begin();
      CCurrentActions::iterator end = pActions->end();

      for (; it != end; it.next())
        success &= it->execute();

      delete pActions;

      CCommunicate::barrierRMA();

      // MPI Broadcast scheduled remote actions and check whether local actions are pending
      broadcastPendingActions();

      // If no local actions are pending anywhere and no remote actions where broadcasted, i.e.,
      // if (Total actions size == 0) break;
    }
  while (TotalPendingActions > 0);

  return success;
}

// static
size_t CActionQueue::pendingActions()
{
  map & ActionQueue = Context.Active().actionQueue;
  map::iterator found = ActionQueue.find(CurrenTick);

  if (found == ActionQueue.end())
    {
      return 0;
    }

  return found->second->size();
}

// static
const CTick & CActionQueue::getCurrentTick()
{
  return CurrenTick;
}

// static
void CActionQueue::setCurrentTick(const int & currentTick)
{
#pragma omp single
  CurrenTick = currentTick;
}

// static
void CActionQueue::incrementTick()
{
#pragma omp single
  ++CurrenTick;
}

// static
void CActionQueue::addRemoteAction(const size_t & actionId, const CNode * pNode)
{
  int index = CNetwork::index(pNode);

  if (index < 0)
    {
      try
        {
#pragma omp critical
          {
            RemoteActions.write(reinterpret_cast< const char * >(&actionId), sizeof(size_t));
            RemoteActions << 'N';
            RemoteActions.write(reinterpret_cast< const char * >(&pNode->id), sizeof(size_t));
          }
        }
      catch (...)
        {
          CLogger::error() << "CActionQueue: Add remote action failed for action '" << index << "' node '" << pNode->id << "'.";
        }
    }
  else
    {
      CActionDefinition * pActionDefinition = CActionDefinition::GetActionDefinition(actionId);
      size_t deltaTick = pActionDefinition->getDelay();
      CAction * pAction = new CNodeAction(pActionDefinition, pNode);

      try
        {
#pragma omp critical
          addAction((Context.beginThread() + index)->locallyAdded, deltaTick, pAction);
        }
      catch (...)
        {
          CLogger::error("CActionQueue: Failed to add action.");
        }
    }
}

// static
void CActionQueue::addRemoteAction(const size_t & actionId, const CEdge * pEdge)
{
  int index = CNetwork::index(pEdge->pTarget);

  if (index < 0)
    {
      try
        {
#pragma omp critical
          {
            RemoteActions.write(reinterpret_cast< const char * >(&actionId), sizeof(size_t));
            RemoteActions << 'E';
            RemoteActions.write(reinterpret_cast< const char * >(&pEdge->targetId), sizeof(size_t));
            RemoteActions.write(reinterpret_cast< const char * >(&pEdge->sourceId), sizeof(size_t));
          }
        }
      catch (...)
        {
          CLogger::error() << "CActionQueue: Add remote action failed for action '" << index << "' edge '" << pEdge->targetId << "', '" << pEdge->sourceId << "'.";
        }
    }
  else
    {
      CActionDefinition * pActionDefinition = CActionDefinition::GetActionDefinition(actionId);
      size_t deltaTick = pActionDefinition->getDelay();
      CAction * pAction = new CEdgeAction(pActionDefinition, pEdge);

      try
        {
#pragma omp critical
          addAction((Context.beginThread() + index)->locallyAdded, deltaTick, pAction);
        }
      catch (...)
        {
          CLogger::error("CActionQueue: Failed to add action.");
        }
    }
}

// static
int CActionQueue::broadcastPendingActions()
{
  sActionQueue & ActionQueue = Context.Active();
  map::const_iterator it = ActionQueue.locallyAdded.begin();
  map::const_iterator end = ActionQueue.locallyAdded.end();

  for (; it != end; ++it)
    {
      CCurrentActions::iterator itAction = it->second->begin(false);
      CCurrentActions::iterator endAction = end->second->end(false);

      for (; itAction != endAction; itAction.next())
        {
          addAction(it->first - CurrenTick, const_cast< CAction * >(*itAction));
        }

      it->second->clear();
    }

  ActionQueue.locallyAdded.clear();

#pragma omp single
  TotalPendingActions = 0;

  size_t PendingActions = pendingActions();

#pragma omp atomic
  TotalPendingActions += PendingActions;

#pragma omp barrier
#pragma omp single
  {
    CLogger::setSingle(true);

    std::ostringstream os;
    os.write(reinterpret_cast< const char * >(&TotalPendingActions), sizeof(size_t));
    os << RemoteActions.str();
    RemoteActions.str("");

    const std::string & Buffer = os.str();

    CCommunicate::Receive Receive(&CActionQueue::receivePendingActions);
    CCommunicate::roundRobin(Buffer.c_str(), Buffer.length(), &Receive);

    CLogger::setSingle(false);
  }
  
  return (int) CCommunicate::ErrorCode::Success;
}

CCommunicate::ErrorCode CActionQueue::receivePendingActions(std::istream & is, int /* sender */)
{
  size_t RemotePendingActions;

  is.read(reinterpret_cast< char * >(&RemotePendingActions), sizeof(size_t));
  TotalPendingActions += RemotePendingActions;

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

      size_t NodeId;
      is.read(reinterpret_cast< char * >(&NodeId), sizeof(size_t));

      if (is.fail())
        break;

      int Index = CNetwork::index(NodeId);

      CNode * pNode = NULL;

      if (Index >= 0)
        pNode = (CNetwork::Context.beginThread() + Index)->lookupNode(NodeId, true);

      switch (TargetType)
        {
        case 'N':
          {
            if (pNode != NULL
                && pActionDefinition != NULL)
              {
                size_t deltaTick = pActionDefinition->getDelay();
                CAction * pAction = new CNodeAction(pActionDefinition, pNode);

                try
                  {
                    addAction((Context.beginThread() + Index)->actionQueue, deltaTick, pAction);
                  }
                catch (...)
                  {
                    CLogger::error("CActionQueue: Failed to add action.");
                  }

                ++TotalPendingActions;
              }
          }

          break;

        case 'E':
          {
            size_t SourceId;
            is.read(reinterpret_cast< char * >(&SourceId), sizeof(size_t));
            if (is.fail())
              break;

            CEdge * pEdge = (CNetwork::Context.beginThread() + Index)->lookupEdge(NodeId, SourceId);

            if (pEdge != NULL
                && pActionDefinition != NULL)
              {
                size_t deltaTick = pActionDefinition->getDelay();
                CAction * pAction = new CEdgeAction(pActionDefinition, pEdge);

                try
                  {
                    addAction((Context.beginThread() + Index)->actionQueue, deltaTick, pAction);
                  }
                catch (...)
                  {
                    CLogger::error("CActionQueue: Failed to add action.");
                  }

                ++TotalPendingActions;
              }
          }
          break;
        }
    }

  return CCommunicate::ErrorCode::Success;
}
