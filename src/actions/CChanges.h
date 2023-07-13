// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2020 - 2023 Rector and Visitors of the University of Virginia 
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
#include "network/CNode.h"
#include "network/CEdge.h"
#include "diseaseModel/CHealthState.h"
#include "utilities/CMetadata.h"

class CChanges
{
public:
  static void init();
  static void release();

  template < class Entity >
  static  void record(const Entity * pEntity, const CMetadata & metadata);

  static void initDefaultOutput();
  static bool writeDefaultOutput();
  static CCommunicate::ErrorCode writeDefaultOutputData();
  static CCommunicate::ErrorCode sendNodesRequested(std::ostream & os, int sender);
  static CCommunicate::ErrorCode determineNodesRequested();
  static CCommunicate::ErrorCode receiveNodesRequested(std::istream & is, int sender);
  static void setCurrentTick(size_t tick);
  static void incrementTick();
  static void reset();

private:
  struct Changes
  {
    std::stringstream *pDefaultOutput;
  };

  static CContext< Changes > Context;
  static std::map< size_t, std::set< const CNode * > > RankToNodesRequested;
  static size_t Tick;
};

template <>
// static 
inline void CChanges::record(const CNode * pNode, const CMetadata & metadata)
  {
    if (pNode == NULL)
      return;

    pNode->changed = true;
    Changes & Active = Context.Active();

    if (metadata.getBool("StateChange"))
      {
        // "tick,pid,exit_state,contact_pid,[locationId]"
        (*Active.pDefaultOutput) << (int) Tick << "," << pNode->id << "," << pNode->getHealthState()->getAnnId() << ",";

        if (metadata.contains("ContactNode"))
          {
            (*Active.pDefaultOutput) << (size_t) metadata.getInt("ContactNode");
          }
        else
          {
            (*Active.pDefaultOutput) << -1;
          }

        if (CEdge::HasLocationId)
          {
            if (metadata.contains("LocationId"))
              {
                (*Active.pDefaultOutput) << "," << (size_t) metadata.getInt("LocationId");
              }
            else
              {
                (*Active.pDefaultOutput) << "," << -1;
              }
          }

        (*Active.pDefaultOutput) << std::endl;
      }
  }

template < class Entity >
// static 
inline void CChanges::record(const Entity * /* pEntity */, const CMetadata & /* metadata */)
{}

#endif /* SRC_ACTIONS_CHANGES_H_ */
