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

#include "math/CComputable.h"
#include "utilities/CLogger.h"

// static
size_t CComputable::UniqueId(0);

CComputable::CComputable()
  : mComputableId(UniqueId++)
  , mStatic(false)
  , mComputedOnce()
  , mPrerequisites()
{
  mComputedOnce.init();
  mComputedOnce.Master() = false;

  bool * pIt = mComputedOnce.beginThread();
  bool * pEnd = mComputedOnce.endThread();

  for (; pIt != pEnd; ++pIt)
    if (mComputedOnce.isThread(pIt))
      *pIt = false;

  Instances.insert(this);
}

CComputable::CComputable(const CComputable & src)
  : mComputableId(UniqueId++)
  , mStatic(src.mStatic)
  , mComputedOnce(src.mComputedOnce)
  , mPrerequisites(src.mPrerequisites)
{
  Instances.insert(this);
}

CComputable::~CComputable()
{
  Instances.erase(this);
}

const CComputableSet & CComputable::getPrerequisites() const
{
  return mPrerequisites;
}

bool CComputable::compute()
{
  if (mStatic && mComputedOnce.Active())
    return true;

  mComputedOnce.Active() = true;

  CLogger::debug() << "CComputable: Computing '" << getComputableId() << "'" <<  (mStatic ? " (static)" : "") << ".";
  return computeProtected();
}

bool CComputable::isStatic() const
{
  return mStatic;
}

void CComputable::determineIsStatic()
{
  mStatic = true;

  CComputableSet::const_iterator it = mPrerequisites.begin();
  CComputableSet::const_iterator end = mPrerequisites.end();

  for (; it != end && mStatic; ++it)
    {
      mStatic = it->second->isStatic();
    }
}

// virtual 
std::string CComputable::getComputableId() const
{
  std::ostringstream os;
  os << mComputableId;

  return os.str();
}
