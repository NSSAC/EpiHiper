/*
 * StreamBuffer.cpp
 *
 *  Created on: Jul 11, 2019
 *      Author: shoops
 */

#include "StreamBuffer.h"

StreamBuffer::StreamBuffer(char * pBuffer, size_t size)
  : std::streambuf()
{
  setg(pBuffer, pBuffer, pBuffer + size);
}

StreamBuffer::~StreamBuffer()
{}

