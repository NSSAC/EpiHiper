// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#ifndef SRC_ACTIONS_CACTIONQUEUE_H_
#define SRC_ACTIONS_CACTIONQUEUE_H_

#include <sstream>

#include "utilities/CCommunicate.h"
#include "actions/CCurrentActions.h"
#include "math/CTick.h"

class CActionQueue: public std::map< int, CCurrentActions * >
{
  typedef std::map< int, CCurrentActions * > base;

  public:
    virtual ~CActionQueue();

    static void init();

    static void release();

    static void addAction(size_t deltaTick, CScheduledAction * pAction);

    static bool processCurrentActions();

    static const CTick & getCurrentTick();

    static void setCurrentTick(const int & currentTick);

    static void incrementTick();

    static size_t pendingActions();

    static void addRemoteAction(const size_t & index, const CNode * pNode);

    static void addRemoteAction(const size_t & index, const CEdge * pEdge);


  private:
    CActionQueue();

    int broadcastPendingActions();

    CCommunicate::ErrorCode receivePendingActions(std::istream & is, int sender);

    static CActionQueue * pINSTANCE;
    CTick mCurrenTick;
    size_t mTotalPendingActions;
    std::stringstream mRemoteActions;
};

#endif /* SRC_ACTIONS_CACTIONQUEUE_H_ */
