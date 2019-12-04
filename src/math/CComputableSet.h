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

#ifndef SRC_MATH_CCOMPUTABLESET_H_
#define SRC_MATH_CCOMPUTABLESET_H_

#include <map>

class CComputable;

class CComputableSet : private std::map< size_t, const CComputable * >
{
public:
  typedef std::map< size_t, const CComputable * >::iterator iterator;
  typedef std::map< size_t, const CComputable * >::const_iterator const_iterator;

  virtual ~CComputableSet();

  iterator begin();

  iterator end();

  const_iterator begin() const;

  const_iterator end() const;

  void insert(const CComputable * pComputable);

  void erase(const CComputable * pComputable);

  void clear();

  bool empty() const;

  size_t size() const;

  const_iterator find(const CComputable * pComputable) const;
};

#endif /* SRC_MATH_CCOMPUTABLESET_H_ */
