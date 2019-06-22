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
project (EpiHiper)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)
set(CMAKE_CXX_STANDARD 11)

find_package (MPI REQUIRED)

include_directories(BEFORE ${MPI_INCLUDE_PATH})
include_directories(BEFORE ${PROJECT_BINARY_DIR}/include)

add_subdirectory(dependencies)
add_subdirectory(src)

message(STATUS "-----------------------------------------------------------
 Building EpiHiper 

   Source Dir        = ${CMAKE_SOURCE_DIR}
   Binary Dir        = ${CMAKE_BINARY_DIR}
   Install Prefix    = ${CMAKE_INSTALL_PREFIX}

 Dependencies:
   MPI Libs          = ${MPI_LIBRARIES}
   MPI LDFLAGS       = ${MPI_LINKER_FLAGS}
   MPI include       = ${MPI_INCLUDE_PATH}
   
   jansson Libs      = ${CMAKE_BINARY_DIR}/lib
   jansson include   = ${CMAKE_BINARY_DIR}/include

-----------------------------------------------------------
")