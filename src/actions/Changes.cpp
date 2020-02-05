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

/*
 * Changes.cpp
 *
 *  Created on: Jul 11, 2019
 *      Author: shoops
 */

#include <fstream>

#include "actions/Changes.h"

#include "utilities/CSimConfig.h"
#include "diseaseModel/CHealthState.h"
#include "network/CEdge.h"
#include "network/CNode.h"
#include "utilities/CMetadata.h"

// static
void Changes::setCurrentTick(size_t tick)
{
  Tick = tick;
}

// static
void Changes::incrementTick()
{
  ++Tick;
}

// static
void Changes::record(const CNode * pNode, const CMetadata & metadata)
{
  if (pNode == NULL)
    return;

  Nodes.insert(pNode);

  if (metadata.getBool("StateChange"))
    {
      // "tick,pid,exit_state,contact_pid"
      DefaultOutput << (int) Tick << "," << pNode->id << "," << pNode->getHealthState()->getAnnId() << ",";

      if (metadata.contains("ContactNode"))
        {
          DefaultOutput << (size_t) metadata.getInt("ContactNode") << std::endl;
        }
      else
        {
          DefaultOutput << -1 << std::endl;
        }
    }
}

// static
void Changes::record(const CEdge * pEdge, const CMetadata & metadata)
{
  if (pEdge == NULL)
    return;

  Edges.insert(pEdge);
}

// static
void Changes::record(const CVariable * pVariable, const CMetadata & metadata)
{}

// static
void Changes::clear()
{
  Nodes.clear();
  Edges.clear();
}

// static
void Changes::initDefaultOutput()
{
  if (CCommunicate::MPIRank == 0)
    {
      std::ofstream out;

      out.open(CSimConfig::getOutput().c_str());

      if (out.good())
        {
          out << "tick,pid,exit_state,contact_pid" << std::endl;
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
void Changes::writeDefaultOutput()
{
  CCommunicate::SequentialProcess WriteData(&Changes::writeDefaultOutputData);
  CCommunicate::sequential(0, &WriteData);

  DefaultOutput.str("");
}

// static
CCommunicate::ErrorCode Changes::writeDefaultOutputData()
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
size_t Changes::size()
{
  return Nodes.size() + Edges.size();
}

// static
const std::ostringstream & Changes::getNodes()
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
const std::ostringstream & Changes::getEdges()
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

Changes::~Changes()
{}
