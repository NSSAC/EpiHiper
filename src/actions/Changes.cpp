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
std::ostringstream Changes::Nodes;

// static
std::ostringstream Changes::Edges;

// static
std::stringstream Changes::DefaultOutput;

// static
size_t Changes::Size = 0;

// static
size_t Changes::Tick = -1;

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
void Changes::record(const CNode & node, const CMetadata & metadata)
{
  node.toBinary(Nodes);

  if (metadata.getBool("StateChange"))
    {
      // "tick,pid,exit_state,contact_pid"
      DefaultOutput << (int) Tick << "," << node.id << "," << node.getHealthState()->getAnnId() << ",";

      if (metadata.contains("ContactNode"))
        {
          DefaultOutput << (size_t) metadata.getInt("ContactNode") << std::endl;
        }
      else
        {
          DefaultOutput << -1 << std::endl;
        }
    }

  ++Size;
}

// static
void Changes::record(const CEdge & edge, const CMetadata & metadata)
{
  edge.toBinary(Edges);
  ++Size;
}

// static
void Changes::record(const CVariable & variable, const CMetadata & metadata)
{}

// static
void Changes::clear()
{
  Size = 0;
  Nodes.str("");
  Edges.str("");
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
          std::cerr << "Error (Rank 0): Failed to open file '" << CSimConfig::getOutput() << "'.";
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
  return Size;
}

// static
const std::ostringstream & Changes::getNodes()
{
  return Nodes;
}

// static
const std::ostringstream & Changes::getEdges()
{
  return Edges;
}

Changes::~Changes()
{}

