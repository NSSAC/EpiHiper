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

#ifndef SRC_ACTIONS_CACTIONQUEUE_H_
#define SRC_ACTIONS_CACTIONQUEUE_H_

#include "utilities/CCommunicate.h"
#include "actions/CCurrentActions.h"

class CActionQueue: public std::map< size_t, CCurrentActions >
{
  public:
    virtual ~CActionQueue();

    static void addAction(size_t deltaTick, const CAction & action);

    static bool processCurrentActions();

    static const size_t & getCurrentTick();

    static void setCurrentTick(const size_t & currentTick);

    static void incrementTick();

    static size_t pendingActions();

  private:
    CActionQueue();

    int broadcastRemoteActions();

    CCommunicate::ErrorCode receiveActions(std::istream & is, int sender);

    static CActionQueue INSTANCE;
    size_t mCurrenTick;
    size_t mTotalPendingActions;
};

#endif /* SRC_ACTIONS_CACTIONQUEUE_H_ */
