// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2020 - 2023 Rector and Visitors of the University of Virginia 
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

#ifndef UTILITIES_CLOGGER_H
#define UTILITIES_CLOGGER_H

#include <sstream>
#include <stack>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/sink.h>
#include "utilities/CContext.h"

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

public : 
  typedef spdlog::level::level_enum LogLevel;

  struct LoggerData
  {
    std::shared_ptr< spdlog::logger > pLogger;
    std::shared_ptr< spdlog::sinks::sink > pConsole;
    std::shared_ptr< spdlog::sinks::sink > pFile;
    std::string task;
    std::stack< spdlog::level::level_enum > levels;
  };

  static void init();

  static void release();

  static void setLevel(LogLevel level);

  static void pushLevel(LogLevel level);

  static void popLevel();

  static void setTask(int rank, int processes);

  static void setLogDir(const std::string dir);
  
  static std::string sanitize(std::string dirty);

  static bool hasErrors();

  static void setSingle(bool single);

  typedef CStream< spdlog::level::trace > trace;
  typedef CStream< spdlog::level::debug > debug;
  typedef CStream< spdlog::level::info > info;
  typedef CStream< spdlog::level::warn > warn;
  typedef CStream< spdlog::level::err > error;
  typedef CStream< spdlog::level::critical > critical;
  typedef CStream< spdlog::level::off > off;

private:
  static void initData(LoggerData & loggerData);
  static void releaseData(LoggerData & loggerData);
  static void setLevel();
  static bool haveErrors;
  static int single;
  static CContext< LoggerData > Context;
};

template < int level >
CLogger::CStream< level >::CStream()
  : std::ostringstream()
{
  if (Context.Active().levels.top() > level)
    setstate(std::ios_base::badbit);
}

template < int level >
CLogger::CStream< level >::CStream(const std::string & msg)
  : std::ostringstream()
{
  if (Context.Active().levels.top() <= level)
    flush(msg);

  setstate(std::ios_base::badbit);
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
  if (bad()
      || tellp() == 0)
    return;

  flush(str());
  str("");
}

// static
template < int level >
void CLogger::CStream< level >::flush(const std::string & msg)
{
  LoggerData * pIt = NULL;
  LoggerData * pEnd = NULL;

  if ((single != -1
       && Context.localIndex(&Context.Active()) == single)
      || omp_get_num_threads() == 1)
    {
      pIt = Context.beginThread();
      pEnd = Context.endThread();
    }
  else
    {
      pIt = &Context.Active();
      pEnd = pIt + 1;
    }

  for (; pIt != pEnd; ++pIt)
    switch (static_cast< LogLevel >(level))
      {
      case spdlog::level::trace:
        pIt->pLogger->trace(pIt->task + " " + msg);
        break;
      case spdlog::level::debug:
        pIt->pLogger->debug(pIt->task + " " + msg);
        break;
      case spdlog::level::info:
        pIt->pLogger->info(pIt->task + " " + msg);
        break;
      case spdlog::level::warn:
        pIt->pLogger->warn(pIt->task + " " + msg);
        break;
      case spdlog::level::err:
  #pragma omp atomic
        haveErrors |= true;
        pIt->pLogger->error(pIt->task + " " + msg);
        break;
      case spdlog::level::critical:
  #pragma omp atomic
        haveErrors |= true;
        pIt->pLogger->critical(pIt->task + " " + msg);
        break;
      case spdlog::level::off:
        break;
      }
}

#endif // UTILITIES_CLOGGER_H