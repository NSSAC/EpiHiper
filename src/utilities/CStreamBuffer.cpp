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
 * StreamBuffer.cpp
 *
 *  Created on: Jul 11, 2019
 *      Author: shoops
 */

#include "CStreamBuffer.h"

CStreamBuffer::CStreamBuffer(char * pBuffer, size_t size)
  : std::streambuf()
{
  setg(pBuffer, pBuffer, pBuffer + size);
}

CStreamBuffer::~CStreamBuffer()
{}

