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
#include "utilities/CCommunicate.h"

class CNode;
class CEdge;
class CVariable;
class CMetadata;


class CChanges
{
public:
  static void record(const CNode * pNode, const CMetadata & metadata);
  static void record(const CEdge * pEdge, const CMetadata & metadata);
  static void record(const CVariable * pVariable, const CMetadata & metadata);
  static void clear();
  static size_t size();

  static const std::ostringstream &  getNodes();
  static const std::ostringstream &  getEdges();

  static void initDefaultOutput();
  static void writeDefaultOutput();
  static CCommunicate::ErrorCode writeDefaultOutputData();
  static void setCurrentTick(size_t tick);
  static void incrementTick();

  CChanges() = delete;
  virtual ~CChanges();

private:
  static std::set< const CNode * > Nodes;
  static std::set< const CEdge * > Edges;
  static std::stringstream DefaultOutput;

  static size_t Tick;
};

#endif /* SRC_ACTIONS_CHANGES_H_ */
