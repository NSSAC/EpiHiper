/*
 * Changes.cpp
 *
 *  Created on: Jul 11, 2019
 *      Author: shoops
 */

#include <fstream>

#include "actions/Changes.h"
#include "network/Node.h"
#include "network/Edge.h"
#include "SimConfig.h"
#include "utilities/Metadata.h"
#include "diseaseModel/State.h"

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
void Changes::record(const Node & node, const Metadata & metadata)
{
  node.toBinary(Nodes);

  if (metadata.getBool("StateChange"))
    {
      // "tick,pid,exit_state,contact_pid"
      DefaultOutput << (int) Tick << "," << node.getData()->id << "," << node.getData()->pHealthState->getAnnId() << ",";

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
void Changes::record(const Edge & edge, const Metadata & metadata)
{
  edge.toBinary(Edges);
  ++Size;
}

// static
void Changes::record(const Variable & variable, const Metadata & metadata)
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
  if (Communicate::Rank == 0)
    {
      std::ofstream out;

      out.open(SimConfig::getOutput().c_str());

      if (out.good())
        {
          out << "tick,pid,exit_state,contact_pid" << std::endl;
        }
      else
        {
          std::cerr << "Error (Rank 0): Failed to open file '" << SimConfig::getOutput() << "'.";
          exit(EXIT_FAILURE);
        }

      out.close();
    }
}

// static
void Changes::writeDefaultOutput()
{
  Communicate::SequentialProcess WriteData(&Changes::writeDefaultOutputData);
  Communicate::sequential(0, &WriteData);

  DefaultOutput.str("");
}

// static
Communicate::ErrorCode Changes::writeDefaultOutputData()
{
  std::ofstream out;

  out.open(SimConfig::getOutput().c_str(), std::ios_base::app);

  if (out.good())
    {
      out << DefaultOutput.str();
    }

  out.close();

  return Communicate::ErrorCode::Success;
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

