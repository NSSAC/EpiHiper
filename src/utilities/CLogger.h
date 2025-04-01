// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2020 - 2024 Rector and Visitors of the University of Virginia 
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

#include <stack>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/sink.h>
#include "utilities/CContext.h"

#ifdef SUPPORT_LOGLEVEL_TRACE
#  define ENABLE_TRACE(trace...) trace
#else
#  define ENABLE_TRACE(trace...)
#endif

class CLogger
{
private:
  template < int LEVEL >
  class CStream
  {
  public:
    CStream() = delete;
    template< class ... Arguments >
    CStream(const std::string & format, Arguments ... arguments);
    virtual ~CStream();
  };

public : 
  typedef spdlog::level::level_enum LogLevel;

  struct LoggerData
  {
    std::shared_ptr< spdlog::logger > pLogger;
    std::shared_ptr< spdlog::sinks::sink > pConsole;
    std::shared_ptr< spdlog::sinks::sink > pFile;
    std::string task;
    std::string tick;
    std::stack< spdlog::level::level_enum > levels;
  };

  static void init();

  static void finalize();

  static void setLevel(LogLevel level);

  static void pushLevel(LogLevel level);

  static void popLevel();

  static LogLevel level();

  static void setTask(int rank, int processes);

  static void setLogDir(const std::string dir);
  
  static std::string sanitize(std::string dirty);

  static bool hasErrors();

  static void setSingle(bool single);

  static void updateTick();

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

template < int LEVEL >
template< class ... Arguments >
  CLogger::CStream< LEVEL >::CStream(const std::string & format, Arguments ... arguments)
{
  if (Context.Active().levels.top() > static_cast< LogLevel >(LEVEL))
    return;

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
    {
      switch (static_cast< LogLevel >(LEVEL))
        {
        case spdlog::level::trace:
          pIt->pLogger->trace(pIt->task + " " + pIt->tick + " " + format, arguments ...);
          break;
        case spdlog::level::debug:
          pIt->pLogger->debug(pIt->task + " " + pIt->tick + " " + format, arguments ...);
          break;
        case spdlog::level::info:
          pIt->pLogger->info(pIt->task + " " + pIt->tick + " " + format, arguments ...);
          break;
        case spdlog::level::warn:
          pIt->pLogger->warn(pIt->task + " " + pIt->tick + " " + format, arguments ...);
          break;
        case spdlog::level::err:
#pragma omp atomic
          haveErrors |= true;
          pIt->pLogger->error(pIt->task + " " + pIt->tick + " " + format, arguments ...);
          break;
        case spdlog::level::critical:
#pragma omp atomic
          haveErrors |= true;
          pIt->pLogger->critical(pIt->task + " " + pIt->tick + " " + format, arguments ...);
          break;
        case spdlog::level::off:
          break;
        }

#ifdef EPIHIPER_DEBUG
      pIt->pLogger->flush();
#else
      if (Context.Active().levels.top() == spdlog::level::trace)
        pIt->pLogger->flush();
#endif // EpiHiper_DEBUG
    }

}

// virtual
template < int LEVEL >
CLogger::CStream< LEVEL >::~CStream()
{}

#endif // UTILITIES_CLOGGER_H