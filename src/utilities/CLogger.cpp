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

#include "utilities/CLogger.h"
#include "utilities/CSimConfig.h"

// static 
std::stack< spdlog::level::level_enum > CLogger::levels;

// static
std::string CLogger::task;

// static 
bool CLogger::haveErrors = false;

// static
void CLogger::init()
{
  // Set global log level to info
  setLevel(spdlog::level::warn);
  std::shared_ptr<spdlog::logger> console = spdlog::stdout_logger_st("console");
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%^%l%$] %v");
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
  spdlog::set_level(level);
}

// static 
void CLogger::popLevel()
{
  levels.pop();

  if (levels.empty())
    spdlog::set_level(CSimConfig::getLogLevel());
  else
    spdlog::set_level(levels.top());
}

// static 
void CLogger::setTask(int rank, int processes)
{
  std::ostringstream os;
  int width = log10(processes) + 1;
  
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
