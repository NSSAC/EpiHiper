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
size_t CActionQueue::Offset = 0;

// static
void CActionQueue::init(const int & firstTick)
{
  Context.init();

  Offset = -firstTick;
  setCurrentTick(firstTick);
}

// static
void CActionQueue::release()
{
  if (Context.size() > 1)
    {
      queue::iterator itAction = Context.Master().actionQueue.begin();
      queue::iterator endAction = Context.Master().actionQueue.end();

      for (; itAction != endAction; ++itAction)
        {
          delete *itAction;
        }

      queue::iterator itLocal = Context.Master().locallyAdded.begin();
      queue::iterator endLocal = Context.Master().locallyAdded.end();

      for (; itLocal != endLocal; ++itLocal)
        {
          delete *itLocal;
        }
    }

  sActionQueue * pActionQueue = Context.beginThread();
  sActionQueue * pEndActionQueue = Context.endThread();

  for (; pActionQueue != pEndActionQueue; ++pActionQueue)
    {
      queue::iterator itAction = pActionQueue->actionQueue.begin();
      queue::iterator endAction = pActionQueue->actionQueue.end();

      for (; itAction != endAction; ++itAction)
        {
          delete *itAction;
        }

      queue::iterator itLocal = pActionQueue->locallyAdded.begin();
      queue::iterator endLocal = pActionQueue->locallyAdded.end();

      for (; itLocal != endLocal; ++itLocal)
        {
          delete *itLocal;
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
void CActionQueue::addAction(CActionQueue::queue & queue, size_t deltaTick, CAction * pAction)
{
  at(CurrenTick + deltaTick, queue)->addAction(pAction);
}

// static
bool CActionQueue::processCurrentActions()
{
  bool success = true;

  // We need to enter the loop at least once
  do
    {
      queue::value_type pActions = at(CurrenTick, Context.Active().actionQueue);
      at(CurrenTick, Context.Active().actionQueue) = new CCurrentActions();

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
  return at(CurrenTick, Context.Active().actionQueue)->size();
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
  queue::iterator it = ActionQueue.locallyAdded.begin();
  queue::iterator end = ActionQueue.locallyAdded.end();

  size_t Tick = -Offset;

  for (; it != end; ++it, ++Tick)
    {
      CCurrentActions::iterator itAction = (*it)->begin(false);
      CCurrentActions::iterator endAction = (*it)->end(false);

      for (; itAction != endAction; itAction.next())
        addAction(Tick - CurrenTick, const_cast< CAction * >(*itAction));

      (*it)->clear();
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

// static
CActionQueue::queue::value_type & CActionQueue::at(size_t index, CActionQueue::queue & queue)
{
  index += Offset;

  while (queue.size() < index + 1)
    queue.push_back(new CCurrentActions());

  return  queue.at(index);
}

