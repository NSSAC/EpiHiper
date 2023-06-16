// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2020 - 2023 Rector and Visitors of the University of Virginia 
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

/*
 * CChanges.cpp
 *
 *  Created on: Jul 11, 2019
 *      Author: shoops
 */

#include <fstream>

#include "actions/CChanges.h"

#include "utilities/CSimConfig.h"
#include "diseaseModel/CHealthState.h"
#include "network/CEdge.h"
#include "network/CNode.h"
#include "network/CNetwork.h"
#include "utilities/CMetadata.h"

// static 
void CChanges::init()
{
  Context.init();
  Context.Master().pDefaultOutput = new std::stringstream;

  Changes * pIt = Context.beginThread();
  Changes * pEnd = Context.endThread();

  for (; pIt != pEnd; ++pIt)
    if (Context.isThread(pIt))
      pIt->pDefaultOutput = new std::stringstream;
}

// static 
void CChanges::release()
{
  delete Context.Master().pDefaultOutput;

  Changes * pIt = Context.beginThread();
  Changes * pEnd = Context.endThread();

  for (; pIt != pEnd; ++pIt)
    if (Context.isThread(pIt))
      delete pIt->pDefaultOutput;

  Context.release();
}

// static
void CChanges::setCurrentTick(size_t tick)
{
  Tick = tick;
}

// static
void CChanges::incrementTick()
{
  ++Tick;
}

// static 
void CChanges::clear()
{
#pragma omp parallel
  {
    CNode * it = CNetwork::Context.Active().beginNode();
    CNode * end = CNetwork::Context.Active().endNode();

    for (; it != end; ++it)
      it->changed = false;
  }
}

// static
void CChanges::record(const CNode * pNode, const CMetadata & metadata)
  {
    if (pNode == NULL)
      return;

    pNode->changed = true;
    Changes & Active = Context.Active();

    if (metadata.getBool("StateChange"))
      {
        // "tick,pid,exit_state,contact_pid,[locationId]"
        (*Active.pDefaultOutput) << (int) Tick << "," << pNode->id << "," << pNode->getHealthState()->getAnnId() << ",";

        if (metadata.contains("ContactNode"))
          {
            (*Active.pDefaultOutput) << (size_t) metadata.getInt("ContactNode");
          }
        else
          {
            (*Active.pDefaultOutput) << -1;
          }

        if (CEdge::HasLocationId)
          {
            if (metadata.contains("LocationId"))
              {
                (*Active.pDefaultOutput) << "," << (size_t) metadata.getInt("LocationId");
              }
            else
              {
                (*Active.pDefaultOutput) << "," << -1;
              }
          }

        (*Active.pDefaultOutput) << std::endl;
      }
  }

  // static
  void CChanges::record(const CEdge * /* pEdge */, const CMetadata & /* metadata */)
  {
    /*
  if (pEdge == NULL)
    return;

  Edges.insert(pEdge);
  */
  }

  // static
  void CChanges::record(const CVariable * /* pVariable */, const CMetadata & /* metadata */)
  {}

  // static
  void CChanges::initDefaultOutput()
  {
    if (CCommunicate::MPIRank == 0)
      {
        std::ofstream out;

        out.open(CSimConfig::getOutput().c_str());

        if (out.good())
          {
            out << "tick,pid,exit_state,contact_pid";

            if (CEdge::HasLocationId)
              out << ",location_id";

            out << std::endl;
          }
        else
          {
            CLogger::error("Error (Rank 0): Failed to open file '" + CSimConfig::getOutput() + "'.");
            exit(EXIT_FAILURE);
          }

        out.close();
      }
  }

  // static
  void CChanges::writeDefaultOutput()
  {
    CCommunicate::SequentialProcess WriteData(&CChanges::writeDefaultOutputData);
    CCommunicate::sequential(0, &WriteData);
  }

  // static
  CCommunicate::ErrorCode CChanges::writeDefaultOutputData()
  {
    std::ofstream out;

    out.open(CSimConfig::getOutput().c_str(), std::ios_base::app);

    if (out.good())
      {
        Changes * pIt = Context.beginThread();
        Changes * pEnd = Context.endThread();

        for (; pIt != pEnd; ++pIt)
          {
            out << pIt->pDefaultOutput->str();
            pIt->pDefaultOutput->str("");
          }
      }

    out.close();

    return CCommunicate::ErrorCode::Success;
  }

  // static
  CCommunicate::ErrorCode CChanges::determineNodesRequested()
  {
    const std::map< size_t, CNode * > & RemoteNodes = CNetwork::Context.Master().getRemoteNodes();
    std::map< size_t, CNode * >::const_iterator it = RemoteNodes.begin();
    std::map< size_t, CNode * >::const_iterator end = RemoteNodes.end();

    size_t * pBuffer = RemoteNodes.size() > 0 ? new size_t[RemoteNodes.size()] : NULL;
    size_t * pId = pBuffer;

    for (; it != end; ++it, ++pId)
      {
        ENABLE_TRACE(CLogger::trace("CChanges: request node '{}'.", it->first););
        *pId = it->first;
      }

    CCommunicate::Receive Receive(&CChanges::receiveNodesRequested);
    CCommunicate::roundRobin(pBuffer, RemoteNodes.size() * sizeof(size_t), &Receive);

    if (pBuffer != NULL)
      delete[] pBuffer;

    return CCommunicate::ErrorCode::Success;
  }

  // static
  CCommunicate::ErrorCode CChanges::sendNodesRequested(std::ostream & os, int receiver)
  {
    size_t Count = 0;

#pragma omp parallel shared(os) reduction(+: Count)
    {
      CNode * it = CNetwork::Context.Active().beginNode();
      CNode * end = CNetwork::Context.Active().endNode();

      std::ostringstream OutStream;

      // std::set< const CNode * >::const_iterator it = Active.__Nodes.begin();
      // std::set< const CNode * >::const_iterator end = Active.__Nodes.end();

      const std::set< const CNode * > & Requested = RankToNodesRequested[receiver];
      std::set< const CNode * >::const_iterator itRequested = Requested.begin();
      std::set< const CNode * >::const_iterator endRequested = Requested.end();

      while (it != end && itRequested != endRequested)
        if (it < *itRequested)
          {
            ++it;
          }
        else if (it > *itRequested)
          {
            ++itRequested;
          }
        else
          {
            ENABLE_TRACE(CLogger::trace("CChanges: send node '{}'.", it->id););

            if (it->changed)
              it->toBinary(OutStream);

            ++it;
            ++itRequested;
            ++Count;
          }

#pragma omp critical
      os << OutStream.str();
    }

  CLogger::debug("CChanges: Sending '{}' nodes to: '{}'.", Count, receiver);

  return CCommunicate::ErrorCode::Success;
}

// static
CCommunicate::ErrorCode CChanges::receiveNodesRequested(std::istream & is, int sender)
{
  std::set< const CNode * > & Requested = RankToNodesRequested[sender];
  Requested.clear();

  CNetwork & Master = CNetwork::Context.Active();
  size_t id;

  while (true)
    {
      is.read(reinterpret_cast< char * >(&id), sizeof(size_t));

      if (is.fail())
        break;

      CNode * pNode = Master.lookupNode(id, true);

      if (pNode != NULL)
        Requested.insert(pNode);
    }

  CLogger::debug("CChanges::receiveNodesRequested: rank {} requested {} of {} ({}\\%)",
                 sender, Requested.size(), Master.getLocalNodeCount(), 100.0 * Requested.size() / Master.getLocalNodeCount());

  return CCommunicate::ErrorCode::Success;
}

