// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2020 - 2022 Rector and Visitors of the University of Virginia 
//  
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal 
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions: 
//  
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software. 
//  
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE 
// END: Copyright 

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
