// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2023 Rector and Visitors of the University of Virginia 
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

#ifndef SRC_ACTIONS_CACTIONQUEUE_H_
#define SRC_ACTIONS_CACTIONQUEUE_H_

#include <sstream>

#include "utilities/CCommunicate.h"
#include "utilities/CContext.h"
#include "actions/CCurrentActions.h"
#include "math/CTick.h"

class CNode;
class CEdge;

class CActionQueue: public std::map< int, CCurrentActions * >
{
  typedef std::map< int, CCurrentActions * > map;

  public:
    static void init();

    static void release();

    static void addAction(size_t deltaTick, CAction * pAction);

    static bool processCurrentActions();

    static const CTick & getCurrentTick();

    static void setCurrentTick(const int & currentTick);

    static void incrementTick();

    static size_t pendingActions();

    static void addRemoteAction(const size_t & index, const CNode * pNode);

    static void addRemoteAction(const size_t & index, const CEdge * pEdge);


  private:
    struct sActionQueue
      {
        map actionQueue;
        map locallyAdded;
      };

    static int broadcastPendingActions();

    static CCommunicate::ErrorCode receivePendingActions(std::istream & is, int sender);

    static CContext< sActionQueue > Context;
    static CTick CurrenTick;
    static size_t TotalPendingActions;
    static std::stringstream RemoteActions;

    static void addAction(map & queue, size_t deltaTick, CAction * pAction);
};

#endif /* SRC_ACTIONS_CACTIONQUEUE_H_ */
