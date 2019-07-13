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

/*
 * CurrentActions.h
 *
 *  Created on: Jul 1, 2019
 *      Author: shoops
 */

#ifndef SRC_ACTIONS_CURRENTACTIONS_H_
#define SRC_ACTIONS_CURRENTACTIONS_H_

#include <map>
#include <vector>

#include <actions/Action.h>

class CurrentActions: protected std::map< double, std::vector< Action > >
{
public:
  typedef std::map< double, std::vector< Action > > base;

  class iterator
  {
  public:
    iterator() = delete;
    iterator(const base & actions, bool begin);

    iterator(const iterator & src);
    ~iterator();

    iterator & next();
    const Action * operator->() const;

    bool operator!=(const iterator & rhs) const;

  private:
    const base * mpBase;
    base::const_iterator mIt;
    std::vector< Action const * > mShuffled;
    std::vector< Action const * >::iterator mItShuffled;
    Action const * mpAction;
  };

  CurrentActions();
  virtual ~CurrentActions();

  void addAction(const Action & action);

  size_t size() const;

  iterator begin();
  iterator end();

private:
};

#endif /* SRC_ACTIONS_CURRENTACTIONS_H_ */
