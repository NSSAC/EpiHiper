// BEGIN: Copyright 
// Copyright (C) 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

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
void CChanges::record(const CNode * pNode, const CMetadata & metadata)
{
  if (pNode == NULL)
    return;

  Nodes.insert(pNode);

  if (metadata.getBool("StateChange"))
    {
      // "tick,pid,exit_state,contact_pid,[locationId]"
      DefaultOutput << (int) Tick << "," << pNode->id << "," << pNode->getHealthState()->getAnnId() << ",";

      if (metadata.contains("ContactNode"))
        {
          DefaultOutput << (size_t) metadata.getInt("ContactNode") << std::endl;
        }
      else
        {
          DefaultOutput << -1 << std::endl;
        }

      if (CEdge::HasLocationId)
        {
          if (metadata.contains("LocationId"))
            {
              DefaultOutput << (size_t) metadata.getInt("LocationId") << std::endl;
            }
          else
            {
              DefaultOutput << -1 << std::endl;
            }
        }
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
void CChanges::clear()
{
  Nodes.clear();
  Edges.clear();
}

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

  DefaultOutput.str("");
}

// static
CCommunicate::ErrorCode CChanges::writeDefaultOutputData()
{
  std::ofstream out;

  out.open(CSimConfig::getOutput().c_str(), std::ios_base::app);

  if (out.good())
    {
      out << DefaultOutput.str();
    }

  out.close();

  return CCommunicate::ErrorCode::Success;
}

// static
CCommunicate::ErrorCode CChanges::determineNodesRequested()
{
  const std::map< size_t, CNode > & RemoteNodes = CNetwork::INSTANCE->getRemoteNodes();
  std::map< size_t, CNode >::const_iterator it = RemoteNodes.begin();
  std::map< size_t, CNode >::const_iterator end = RemoteNodes.end();

  size_t * pBuffer = RemoteNodes.size() > 0 ? new size_t[RemoteNodes.size()] : NULL;
  size_t * pId = pBuffer;

  for (; it != end; ++it, ++pId)
    *pId = it->first;

  CCommunicate::Receive Receive(&CChanges::receiveNodesRequested);
  CCommunicate::roundRobin(pBuffer, RemoteNodes.size() * sizeof(size_t), &Receive);

  if (pBuffer != NULL)
    delete[] pBuffer;

  return CCommunicate::ErrorCode::Success;
}

// static 
CCommunicate::ErrorCode CChanges::sendNodesRequested(std::ostream & os, int receiver)
{
  std::set< const CNode * >::const_iterator it = Nodes.begin();
  std::set< const CNode * >::const_iterator end = Nodes.end();

  const std::set< const CNode * > & Requested = RankToNodesRequested[receiver];
  std::set< const CNode * >::const_iterator itRequested = Requested.begin();
  std::set< const CNode * >::const_iterator endRequested = Requested.end();

  while (it != end && itRequested != endRequested)
    if (*it < *itRequested)
      {
        ++it;
      }
    else if (*it > *itRequested)
      {
        ++itRequested;
      }
    else
      {
        (*it)->toBinary(os);
        ++it;
        ++itRequested;
      }

  return CCommunicate::ErrorCode::Success;
}

// static
CCommunicate::ErrorCode CChanges::receiveNodesRequested(std::istream & is, int sender)
{
  std::set< const CNode * > & Requested = RankToNodesRequested[sender];
  Requested.clear();
  size_t id;

  while (true)
    {
      is.read(reinterpret_cast< char * >(&id), sizeof(size_t));

      if (is.fail())
        break;

      CNode * pNode = CNetwork::INSTANCE->lookupNode(id, true);

      if (pNode != NULL)
        Requested.insert(pNode);
    }

  CLogger::debug() << "CChanges::receiveNodesRequested: rank "
                   << sender << " requested " << Requested.size() << " of "
                   << CNetwork::INSTANCE->getLocalNodeCount() << " ("
                   << 100.0 * Requested.size() / CNetwork::INSTANCE->getLocalNodeCount() << ")";

  return CCommunicate::ErrorCode::Success;
}

// static
size_t CChanges::size()
{
  return Nodes.size() + Edges.size();
}

// static
const std::ostringstream & CChanges::getNodes()
{
  static std::ostringstream os;

  os.str("");

  std::set< const CNode * >::const_iterator it = Nodes.begin();
  std::set< const CNode * >::const_iterator end = Nodes.end();

  for (; it != end; ++it)
    {
      (*it)->toBinary(os);
    }

  return os;
}

// static
const std::ostringstream & CChanges::getEdges()
{
  static std::ostringstream os;

  os.str("");

  std::set< const CEdge * >::const_iterator it = Edges.begin();
  std::set< const CEdge * >::const_iterator end = Edges.end();

  for (; it != end; ++it)
    {
      (*it)->toBinary(os);
    }

  return os;
}

CChanges::~CChanges()
{}
