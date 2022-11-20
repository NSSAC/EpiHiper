// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
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
