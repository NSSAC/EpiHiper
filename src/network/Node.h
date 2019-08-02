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
 * Node.h
 *
 *  Created on: Jun 26, 2019
 *      Author: shoops
 */

#ifndef SRC_NETWORK_NODE_H_
#define SRC_NETWORK_NODE_H_

#include <iostream>
#include "traits/TraitData.h"

class State;
class Transmission;
class Progression;
class Metadata;

class CEdge;

struct NodeData {
  size_t id;
  const State * pHealthState;
  double susceptibilityFactor;
  double susceptibility;
  double infectivityFactor;
  double infectivity;
  TraitData::base nodeTrait;
  CEdge * Edges;
  size_t EdgesSize;

  NodeData();
};

class Node
{
public:
  Node() = delete;
  Node(NodeData * pData);
  Node(const Node & src);

  virtual ~Node();

  const NodeData * getData() const;
  void toBinary(std::ostream & os) const;
  void fromBinary(std::istream & is);

  static void toBinary(std::ostream & os, const NodeData * pNode);
  static void fromBinary(std::istream & is, NodeData * pNode);
  static NodeData getDefault();

  bool set(const Transmission * pTransmission, const Metadata & metadata);

  bool set(const Progression * pProgression, const Metadata & metadata);

private:
  NodeData * mpData;
};

#endif /* SRC_NETWORK_NODE_H_ */
