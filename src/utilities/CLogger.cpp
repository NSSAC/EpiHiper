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

#include <spdlog/sinks/stdout_sinks.h>

#include "utilities/CLogger.h"
#include "utilities/CSimConfig.h"
// static
void CLogger::init()
{
  // Set global log level to info
  spdlog::set_level(CSimConfig::getLogLevel());

  std::shared_ptr<spdlog::logger> console = spdlog::stdout_logger_st("console");
}

// static
void CLogger::release()
{
  spdlog::shutdown();
}