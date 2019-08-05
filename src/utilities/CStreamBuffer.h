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
 * StreamBuffer.h
 *
 *  Created on: Jul 11, 2019
 *      Author: shoops
 */

#ifndef SRC_UTILITIES_CSTREAMBUFFER_H_
#define SRC_UTILITIES_CSTREAMBUFFER_H_

#include <istream>
#include <iostream>
#include <streambuf>
#include <string>
#include <string.h>

class CStreamBuffer: public std::streambuf
{
public:
  CStreamBuffer() = delete;
  CStreamBuffer(char * pBuffer, size_t size);

  virtual ~CStreamBuffer();
};

#endif /* SRC_UTILITIES_CSTREAMBUFFER_H_ */
