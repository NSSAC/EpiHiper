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
 * CChanges.h
 *
 *  Created on: Jul 11, 2019
 *      Author: shoops
 */

#ifndef SRC_ACTIONS_CHANGES_H_
#define SRC_ACTIONS_CHANGES_H_

#include <sstream>
#include <set>
#include <map>

#include "utilities/CCommunicate.h"
#include "utilities/CContext.h"

class CNode;
class CEdge;
class CVariable;
class CMetadata;


class CChanges
{
public:
  static void init();
  static void release();
  static void record(const CNode * pNode, const CMetadata & metadata);
  static void record(const CEdge * pEdge, const CMetadata & metadata);
  static void record(const CVariable * pVariable, const CMetadata & metadata);

  static void initDefaultOutput();
  static void writeDefaultOutput();
  static CCommunicate::ErrorCode writeDefaultOutputData();
  static CCommunicate::ErrorCode sendNodesRequested(std::ostream & os, int sender);
  static CCommunicate::ErrorCode determineNodesRequested();
  static CCommunicate::ErrorCode receiveNodesRequested(std::istream & is, int sender);
  static void setCurrentTick(size_t tick);
  static void incrementTick();
  static void clear();

private:
  struct Changes
  {
    std::set< const CNode * > Nodes;
    std::set< const CEdge * > Edges;
    std::stringstream *pDefaultOutput;
  };

  static CContext< Changes > Context;
  static std::map< size_t, std::set< const CNode * > > RankToNodesRequested;
  static size_t Tick;
};

#endif /* SRC_ACTIONS_CHANGES_H_ */
