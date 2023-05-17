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

#ifndef SRC_ACTIONS_COPERATION_H_
#define SRC_ACTIONS_COPERATION_H_

#include "actions/CChanges.h"
#include "utilities/CMetadata.h"
#include "math/CValueInterface.h"
#include "sets/CSetCollector.h"

struct COperation
{
  template < class Target, class Value >
  static bool execute(Target * pTarget,
               Value value,
               CValueInterface::pOperator pOperator,
               bool (Target::*method)(Value, CValueInterface::pOperator, const CMetadata &),
               std::set< std::shared_ptr< CSetCollectorInterface > > & collectors,
               const CMetadata & metadata = CMetadata())
  {
    bool changed = (pTarget->*method)(value, pOperator, metadata);

    if (changed)
      {
        CChanges::record(pTarget, metadata);

        for (const std::shared_ptr< CSetCollectorInterface > & collector : collectors)
          collector->record(pTarget);
      }

    return changed;
  };

  template < class Target, class Value >
  static bool execute(Target * pTarget,
               Value value,
               CValueInterface::pOperator pOperator,
               bool (Target::*method)(Value, CValueInterface::pOperator, const CMetadata &),
               const CMetadata & metadata = CMetadata())
  {
    bool changed = (pTarget->*method)(value, pOperator, metadata);

    if (changed)
      {
        CChanges::record(pTarget, metadata);
      }

    return changed;
  };
};

#endif /* SRC_ACTIONS_COPERATION_H_ */
