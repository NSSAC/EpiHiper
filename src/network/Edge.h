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

#ifndef SRC_NETWORK_EDGE_H_
#define SRC_NETWORK_EDGE_H_

#include <iostream>
#include "traits/TraitData.h"

struct NodeData;

struct EdgeData {
  size_t targetId;
  TraitData::base targetActivity;
  size_t sourceId;
  TraitData::base sourceActivity;
  double duration;
  TraitData::base edgeTrait;
  bool active;
  double weight;
  NodeData * pTarget;
  NodeData * pSource;

  EdgeData();
};

class Edge
{
  public:
    Edge() = delete;
    Edge(EdgeData * pData);
    Edge(const Edge & src);
    virtual ~Edge();

    void toBinary(std::ostream & os) const;
    void fromBinary(std::istream & is);

    static bool HasEdgeTrait;
    static bool HasActiveField;
    static bool HasWeightField;

    static void toBinary(std::ostream & os, const EdgeData * pEdge);
    static void fromBinary(std::istream & is, EdgeData * pEdge);
    static EdgeData getDefault();

  private:
    EdgeData * mpData;
};

#endif /* SRC_NETWORK_EDGE_H_ */
