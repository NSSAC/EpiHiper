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
struct EdgeData;

struct NodeData {
  size_t id;
  const State * pHealthState;
  double susceptibilityFactor;
  double susceptibility;
  double infectivityFactor;
  double infectivity;
  TraitData::base nodeTrait;
  EdgeData * Edges;
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

  static void toBinary(std::ostream & os, const NodeData * pNode);
  static void fromBinary(std::istream & is, NodeData * pNode);
  static NodeData getDefault();

  bool set(const State * pState);

private:
  NodeData * mpData;
};

#endif /* SRC_NETWORK_NODE_H_ */
