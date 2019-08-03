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

