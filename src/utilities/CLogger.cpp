// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2020 - 2022 Rector and Visitors of the University of Virginia 
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

#include <iomanip>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "utilities/CLogger.h"
#include "utilities/CSimConfig.h"

// static 
bool CLogger::haveErrors = false;

// static 
int CLogger::single = -1;

// static 
CContext< CLogger::LoggerData > CLogger::Context = CContext< CLogger::LoggerData >();

// static
void CLogger::init()
{
  Context.init();

  initData(Context.Master());

  LoggerData * pIt = Context.beginThread();
  LoggerData * pEnd = Context.endThread();

  for (; pIt != pEnd; ++pIt)
    if (Context.isThread(pIt))
      initData(*pIt);

  spdlog::set_default_logger(Context.beginThread()->pLogger);
  spdlog::flush_every(std::chrono::seconds(5));
}

// static 
void CLogger::initData(LoggerData & loggerData)
{
  loggerData.task = "";
  loggerData.levels = std::stack< LogLevel >();
  loggerData.levels.push(spdlog::level::warn);

  loggerData.pConsole = std::make_shared< spdlog::sinks::stdout_sink_st >();
  loggerData.pConsole->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%^%l%$] %v");
  loggerData.pConsole->set_level(loggerData.levels.top());

  loggerData.pLogger = std::make_shared< spdlog::logger >("Multi Sink", loggerData.pConsole);
  loggerData.pLogger->set_level(loggerData.levels.top());

  loggerData.pFile = NULL;
}

// static 
void CLogger::releaseData(LoggerData & loggerData)
{
  loggerData.pConsole = NULL;
  loggerData.pLogger = NULL;
  loggerData.pFile = NULL;
}

// static
void CLogger::release()
{
  spdlog::shutdown();
}

// static 
void CLogger::setLevel(LogLevel level)
{
  LoggerData & Active = Context.Active();
  Active.levels = std::stack< LogLevel >();

  if (Context.isMaster(&Context.Active()))
    {
      LoggerData * pIt = Context.beginThread();
      LoggerData * pEnd = Context.endThread();

      for (; pIt != pEnd; ++pIt)
        if (Context.isThread(pIt))
          pIt->levels = std::stack< LogLevel >();
    }

  pushLevel(level);
}

// static 
void CLogger::pushLevel(LogLevel level)
{
  LoggerData & Active = Context.Active();
  Active.levels.push(level);

  if (Context.isMaster(&Active))
    {
      LoggerData * pIt = Context.beginThread();
      LoggerData * pEnd = Context.endThread();

      for (; pIt != pEnd; ++pIt)
        if (Context.isThread(pIt))
          pIt->levels.push(level);
    }

  setLevel();
}

// static 
void CLogger::popLevel()
{
  LoggerData & Active = Context.Active();
  Active.levels.pop();

  if (Active.levels.empty())
    Active.levels.push(CSimConfig::getLogLevel());

  if (Context.isMaster(&Active))
    {
      LoggerData * pIt = Context.beginThread();
      LoggerData * pEnd = Context.endThread();

      for (; pIt != pEnd; ++pIt)
        if (Context.isThread(pIt))
          {
            pIt->levels.pop();

            if (pIt->levels.empty())
              pIt->levels.push(CSimConfig::getLogLevel());
          }
    }

  setLevel();
}

// static 
void CLogger::setLevel()
{
  LoggerData & Active = Context.Active();

  Active.pLogger->set_level(Active.levels.top());
  Active.pConsole->set_level(std::max(Active.levels.top(), spdlog::level::warn));

  if (Active.pFile != NULL)
    Active.pFile->set_level(Active.levels.top());

  if (Context.isMaster(&Active))
    {
      LoggerData * pIt = Context.beginThread();
      LoggerData * pEnd = Context.endThread();

      for (; pIt != pEnd; ++pIt)
        if (Context.isThread(pIt))
          {
            pIt->pLogger->set_level(pIt->levels.top());
            pIt->pConsole->set_level(std::max(pIt->levels.top(), spdlog::level::warn));

            if (pIt->pFile != NULL)
              pIt->pFile->set_level(pIt->levels.top());
          }
    }
}

// static 
void CLogger::setLogDir(const std::string dir)
{
  LoggerData & Active = Context.Active();

  if (Context.isThread(&Active))
    {
      CLogger::error() << "CLogger::setLogDir: This function may only be called from master.";
      return;
    }

  if (Active.levels.top() >= spdlog::level::warn)
    return;
  
  if (Context.size() == 1)
    {
      Active.pFile = std::make_shared< spdlog::sinks::basic_file_sink_st >(dir + "." + Active.task + ".log", true);
      Active.pFile->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%^%l%$] %v");
      Active.pFile->set_level(Active.levels.top());
      Active.pLogger->sinks().push_back(Active.pFile);
    }
  else
    {
      LoggerData * pIt = Context.beginThread();
      LoggerData * pEnd = Context.endThread();

      for (; pIt != pEnd; ++pIt)
        if (Context.isThread(pIt))
          {
            pIt->pFile = std::make_shared< spdlog::sinks::basic_file_sink_st >(dir + "." + pIt->task + ".log", true);
            pIt->pFile->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%^%l%$] %v");
            pIt->pFile->set_level(pIt->levels.top());
            pIt->pLogger->sinks().push_back(pIt->pFile);
          }
    }
  
  setLevel();
}

// static 
void CLogger::setTask(int rank, int processes)
{
  LoggerData & Active = Context.Active();

  if (Context.isThread(&Active))
    {
      CLogger::error() << "CLogger::setTask: This function may only be called from master.";
      return;
    }

  int width = log10(processes) + 1;

  std::ostringstream os;
  os << "[" << processes << ":" << std::setfill('0') << std::setw(width) << rank << "]";

  if (Context.size() == 1)
    {
      Active.task = os.str();
    }
  else
    {
      std::string Task = os.str();
      width = log10(Context.size()) + 1;
      LoggerData * pIt = Context.beginThread();
      LoggerData * pEnd = Context.endThread();

      for (; pIt != pEnd; ++pIt)
        if (Context.isThread(pIt))
          {
            std::ostringstream os;
            os << Task << "[" << Context.size() << ":" << std::setfill('0') << std::setw(width) << Context.localIndex(pIt) << "]";
            pIt->task = os.str();
          }
    }
}

// static 
void CLogger::setSingle(bool s)
{
  int index = s ? Context.localIndex(&Context.Active()) : -1;

#pragma omp atomic write
  single = index;
}

// static 
bool CLogger::hasErrors()
{
  return haveErrors;
}

// static 
std::string CLogger::sanitize(std::string dirty)
{
  std::replace(dirty.begin(), dirty.end(), '\n', ' ');
  std::replace(dirty.begin(), dirty.end(), '\t', ' ');
  return dirty;
}
