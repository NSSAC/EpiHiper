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
