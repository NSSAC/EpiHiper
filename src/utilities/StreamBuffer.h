/*
 * StreamBuffer.h
 *
 *  Created on: Jul 11, 2019
 *      Author: shoops
 */

#ifndef SRC_UTILITIES_STREAMBUFFER_H_
#define SRC_UTILITIES_STREAMBUFFER_H_

#include <istream>
#include <iostream>
#include <streambuf>
#include <string>
#include <string.h>

class StreamBuffer: public std::streambuf
{
public:
  StreamBuffer() = delete;
  StreamBuffer(char * pBuffer, size_t size);

  virtual ~StreamBuffer();
};

#endif /* SRC_UTILITIES_STREAMBUFFER_H_ */
