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

#include "diseaseModel/Model.h"
#include "diseaseModel/State.h"
#include "traits/Trait.h"

#include "Node.h"

// static
void Node::toBinary(std::ostream & os, const NodeData * pNode)
{
  size_t Index = Model::stateToIndex(pNode->pHealthState);

  os.write(reinterpret_cast<const char *>(&pNode->id), sizeof(size_t));
  os.write(reinterpret_cast<const char *>(&Index), sizeof(size_t));
  os.write(reinterpret_cast<const char *>(&pNode->susceptibilityFactor), sizeof(double));
  os.write(reinterpret_cast<const char *>(&pNode->susceptibility), sizeof(double));
  os.write(reinterpret_cast<const char *>(&pNode->infectivityFactor), sizeof(double));
  os.write(reinterpret_cast<const char *>(&pNode->infectivity), sizeof(double));
  os.write(reinterpret_cast<const char *>(&pNode->nodeTrait), sizeof(TraitData::base));
}

// static
void Node::fromBinary(std::istream & is, NodeData * pNode)
{
  size_t Index;

  is.read(reinterpret_cast<char *>(&pNode->id), sizeof(size_t));
  is.read(reinterpret_cast<char *>(&Index), sizeof(size_t));
  is.read(reinterpret_cast<char *>(&pNode->susceptibilityFactor), sizeof(double));
  is.read(reinterpret_cast<char *>(&pNode->susceptibility), sizeof(double));
  is.read(reinterpret_cast<char *>(&pNode->infectivityFactor), sizeof(double));
  is.read(reinterpret_cast<char *>(&pNode->infectivity), sizeof(double));
  is.read(reinterpret_cast<char *>(&pNode->nodeTrait), sizeof(TraitData::base));

  pNode->pHealthState = Model::stateFromIndex(Index);
}

// static
NodeData Node::getDefault()
{
  NodeData Default;

  Default.id = -1;
  Default.pHealthState = &Model::getInitialState();
  Default.susceptibilityFactor = 1.0;
  Default.susceptibility = Default.pHealthState->getSusceptibility();
  Default.infectivityFactor = 1.0;
  Default.infectivity = Default.pHealthState->getInfectivity();
  Default.nodeTrait = Trait::NodeTrait->getDefault();
  Default.Edges = NULL;
  Default.EdgesSize = 0;

  return Default;
}

Node::~Node()
{}

