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

#include "math/CComputableSet.h"
#include "math/CComputable.h"

// virtual
CComputableSet::~CComputableSet()
{}

CComputableSet::iterator CComputableSet::begin()
{
  return std::map< size_t, const CComputable * >::begin();
}

CComputableSet::iterator CComputableSet::end()
{
  return std::map< size_t, const CComputable * >::end();
}

CComputableSet::const_iterator CComputableSet::begin() const
{
  return std::map< size_t, const CComputable * >::begin();
}

CComputableSet::const_iterator CComputableSet::end() const
{
  return std::map< size_t, const CComputable * >::end();
}

void CComputableSet::insert(const CComputable * pComputable)
{
  std::map< size_t, const CComputable * >::insert(std::make_pair(pComputable->mComputableId, pComputable));
}

void CComputableSet::erase(const CComputable * pComputable)
{
  std::map< size_t, const CComputable * >::erase(pComputable->mComputableId);
}

void CComputableSet::clear()
{
  std::map< size_t, const CComputable * >::clear();
}

bool CComputableSet::empty() const
{
  return std::map< size_t, const CComputable * >::empty();
}

size_t CComputableSet::size() const
{
  return std::map< size_t, const CComputable * >::size();
}

CComputableSet::const_iterator CComputableSet::find(const CComputable * pComputable) const
{
  return  std::map< size_t, const CComputable * >::find(pComputable->mComputableId);
}


