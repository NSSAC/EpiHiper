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

#ifndef SRC_ACTIONS_ACTIONQUEUE_H_
#define SRC_ACTIONS_ACTIONQUEUE_H_

#include "actions/CurrentActions.h"
#include "utilities/Communicate.h"

class ActionQueue: public std::map< size_t, CurrentActions >
{
  public:
    virtual ~ActionQueue();

    static void addAction(size_t deltaTick, const Action & action);

    static bool processCurrentActions();

    static void setCurrentTick(const size_t & currentTick);

    static void incrementTick();

    static size_t pendingActions();

  private:
    ActionQueue();

    int broadcastRemoteActions();

    Communicate::ErrorCode receiveActions(std::istream & is, int sender);

    static ActionQueue INSTANCE;
    size_t mCurrenTick;
    size_t mTotalPendingActions;
};

#endif /* SRC_ACTIONS_ACTIONQUEUE_H_ */
