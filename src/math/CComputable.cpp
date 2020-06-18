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

#include "math/CComputable.h"
#include "utilities/CLogger.h"

// static
size_t CComputable::UniqueId(0);

CComputable::CComputable()
  : mComputableId(UniqueId++)
  , mStatic(false)
  , mComputedOnce(false)
  , mPrerequisites()
{
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
  if (mStatic && mComputedOnce)
    return true;

  mComputedOnce = true;

  CLogger::debug() << "CComputable: Computing '" << mComputableId << "'" <<  (mStatic ? " (static)" : "") << ".";
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
