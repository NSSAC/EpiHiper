# BEGIN: Copyright 
# Copyright (C) 2019 Rector and Visitors of the University of Virginia 
# All rights reserved 
# END: Copyright 

# BEGIN: License 
# Licensed under the Apache License, Version 2.0 (the "License"); 
# you may not use this file except in compliance with the License. 
# You may obtain a copy of the License at 
#   http://www.apache.org/licenses/LICENSE-2.0 
# END: License 

cmake_minimum_required (VERSION 3.11)

include(ExternalProject)

ExternalProject_Add(jansson
  PREFIX            ${CMAKE_BINARY_DIR}/dependencies
  SOURCE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}/jansson
  
  CONFIGURE_COMMAND  cmake -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/jansson
  BUILD_COMMAND      make 
  INSTALL_COMMAND    make install
)