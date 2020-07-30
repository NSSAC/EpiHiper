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

#include "actions/CAction.h"

class CCurrentActions: protected std::map< double, std::vector< CAction * > >
{
public:
  typedef std::map< double, std::vector< CAction * > > base;

  class iterator
  {
  public:
    iterator() = delete;
    iterator(const base & actions, bool begin, bool shuffle);

    iterator(const iterator & src);
    ~iterator();

    iterator & next();
    const CAction * operator->() const;
    const CAction * operator*();
    bool operator!=(const iterator & rhs) const;

  private:
    const base * mpBase;
    base::const_iterator mIt;
    std::vector< CAction const * > mShuffled;
    std::vector< CAction const * >::iterator mItShuffled;
    CAction const * mpAction;
    bool mShuffle;
  };

  CCurrentActions();

  virtual ~CCurrentActions();

  void addAction(CAction * pAction);

  size_t size() const;

  iterator begin(bool shuffle = true);
  iterator end(bool shuffle = true);

  void clear();

private:
};

#endif /* SRC_ACTIONS_CCURRENTACTIONS_H_ */
