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

#include <iomanip>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "utilities/CLogger.h"
#include "utilities/CSimConfig.h"

// static 
std::stack< spdlog::level::level_enum > CLogger::levels;

// static
std::string CLogger::task;

// static 
bool CLogger::haveErrors = false;

// static 
std::shared_ptr< spdlog::logger >  CLogger::pLogger = std::shared_ptr< spdlog::logger >(); 

// static 
std::shared_ptr< spdlog::sinks::sink > CLogger::pConsole = std::shared_ptr< spdlog::sinks::sink >();

// static 
std::shared_ptr< spdlog::sinks::sink > CLogger::pFile = std::shared_ptr< spdlog::sinks::sink >();

// static
void CLogger::init()
{
  // Set global log level to info
  pConsole = std::make_shared< spdlog::sinks::stdout_sink_st >();
  pConsole->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%^%l%$] %v");
  setLevel(spdlog::level::warn);

  pLogger = std::make_shared< spdlog::logger >("Multi Sink", pConsole);
  pLogger->set_level(spdlog::level::trace);
  
  spdlog::set_default_logger(pLogger);
  spdlog::flush_every(std::chrono::seconds(5));
}

// static
void CLogger::release()
{
  spdlog::shutdown();
}

// static 
void CLogger::setLevel(spdlog::level::level_enum level)
{
  levels = std::stack< spdlog::level::level_enum >();
  pushLevel(level);
}

// static 
void CLogger::pushLevel(spdlog::level::level_enum level)
{
  levels.push(level);
  setLevel();
}

// static 
void CLogger::popLevel()
{
  levels.pop();

  if (levels.empty())
    pushLevel(CSimConfig::getLogLevel());
  else
    setLevel();
}

// static 
void CLogger::setLevel()
{
  if (pConsole)
    pConsole->set_level(std::max(levels.top(), spdlog::level::warn));

  if (pFile)
    pFile->set_level(levels.top());
}

// static 
void CLogger::setLogDir(const std::string dir)
{
  if (task.empty())
    task = "[1:0] ";

  pFile = std::make_shared< spdlog::sinks::basic_file_sink_st >(dir + "." + task.substr(0, task.length() - 1) + ".log", true);
  pFile->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%^%l%$] %v");
  pLogger->sinks().push_back(pFile);

  setLevel();
}

// static 
void CLogger::setTask(int rank, int processes)
{
  int width = log10(processes) + 1;

  std::ostringstream os;
  os << "[" << processes << ":" << std::setfill('0') << std::setw(width) << rank << "] ";
  task = os.str();
}

// static 
bool CLogger::hasErrors()
{
  return haveErrors;
}

// static 
const std::string CLogger::sanitize(std::string dirty)
{
  std::replace(dirty.begin(), dirty.end(), '\n', ' ');
  std::replace(dirty.begin(), dirty.end(), '\t', ' ');
  return dirty;
}
