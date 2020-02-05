// BEGIN: Copyright 
// Copyright (C) 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#ifndef UTILITIES_CLOGGER_H
#define UTILITIES_CLOGGER_H

#include <sstream>
#include <spdlog/spdlog.h>

class CLogger
{
private:
  template < int level >
  class CStream : public std::ostringstream
  {
  public:
    CStream();
    CStream(const std::string & msg);
    virtual ~CStream();
    void flush();

  private:
    static void flush(const std::string & msg);
  };

public:
  static void init();

  static void release();

  typedef CStream< spdlog::level::trace > trace;
  typedef CStream< spdlog::level::debug > debug;
  typedef CStream< spdlog::level::info > info;
  typedef CStream< spdlog::level::warn > warn;
  typedef CStream< spdlog::level::err > error;
  typedef CStream< spdlog::level::critical > critical;
  typedef CStream< spdlog::level::off > off;
};

template < int level >
CLogger::CStream< level >::CStream()
  : std::ostringstream()
{}

template < int level >
CLogger::CStream< level >::CStream(const std::string & msg)
  : std::ostringstream()
{
  flush(msg);
}

// virtual
template < int level >
CLogger::CStream< level >::~CStream()
{
  flush();
}

template < int level >
void CLogger::CStream< level >::flush()
{
  if (tellp() != 0)
    {
      flush(str());
      str("");
    }
}

// static
template < int level >
void CLogger::CStream< level >::flush(const std::string & msg)
{
  switch (static_cast< spdlog::level::level_enum >(level))
    {
    case spdlog::level::trace:
      spdlog::trace(msg);
      break;
    case spdlog::level::debug:
      spdlog::debug(msg);
      break;
    case spdlog::level::info:
      spdlog::info(msg);
      break;
    case spdlog::level::warn:
      spdlog::warn(msg);
      break;
    case spdlog::level::err:
      spdlog::error(msg);
      break;
    case spdlog::level::critical:
      spdlog::critical(msg);
      break;
    case spdlog::level::off:
      break;
    }
}

#endif // UTILITIES_CLOGGER_H