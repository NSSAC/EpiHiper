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

/*
 * CurrentActions.h
 *
 *  Created on: Jul 1, 2019
 *      Author: shoops
 */

#ifndef SRC_ACTIONS_CCURRENTACTIONS_H_
#define SRC_ACTIONS_CCURRENTACTIONS_H_

#include <map>
#include <vector>

#include "actions/CScheduledAction.h"

class CCurrentActions: protected std::map< double, std::vector< CScheduledAction * > >
{
public:
  typedef std::map< double, std::vector< CScheduledAction * > > base;

  class iterator
  {
  public:
    iterator() = delete;
    iterator(const base & actions, bool begin);

    iterator(const iterator & src);
    ~iterator();

    iterator & next();
    const CScheduledAction * operator->() const;

    bool operator!=(const iterator & rhs) const;

  private:
    const base * mpBase;
    base::const_iterator mIt;
    std::vector< CScheduledAction const * > mShuffled;
    std::vector< CScheduledAction const * >::iterator mItShuffled;
    CScheduledAction const * mpAction;
  };

  CCurrentActions();

  virtual ~CCurrentActions();

  void addAction(CScheduledAction * pAction);

  size_t size() const;

  iterator begin();
  iterator end();

private:
};

#endif /* SRC_ACTIONS_CCURRENTACTIONS_H_ */
