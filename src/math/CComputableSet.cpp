// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
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

#include <iostream>

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


