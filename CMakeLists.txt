# BEGIN: Copyright 
# MIT License 
#  
# Copyright (C) 2019 - 2023 Rector and Visitors of the University of Virginia 
#  
# Permission is hereby granted, free of charge, to any person obtaining a copy 
# of this software and associated documentation files (the "Software"), to deal 
# in the Software without restriction, including without limitation the rights 
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
# copies of the Software, and to permit persons to whom the Software is 
# furnished to do so, subject to the following conditions: 
#  
# The above copyright notice and this permission notice shall be included in all 
# copies or substantial portions of the Software. 
#  
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
# SOFTWARE 
# END: Copyright 

cmake_minimum_required (VERSION 3.11)
cmake_policy(SET CMP0074 NEW)

project (EpiHiper VERSION 2.0.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)
set(CMAKE_CXX_STANDARD 11)

if (APPLE)
  set(TARGET_MACOSX 1)

  # Add homebrew specific library path
  find_program(HOMEBREW brew QUIET)

  if (HOMEBREW)
    string(REPLACE "/bin/brew" "/opt" HOMEBREW ${HOMEBREW})
    set(PostgreSQL_ROOT ${PostgreSQL_ROOT} "${HOMEBREW}/libpq")
    set(OpenMP_ROOT ${OpenMP_ROOT} "${HOMEBREW}/libomp")
  endif (HOMEBREW)
endif(APPLE)

option(ENABLE_LOCATION_ID "Enable the Location Id field in the network" ON)

if (ENABLE_LOCATION_ID)
  set(USE_LOCATION_ID 1)
endif(ENABLE_LOCATION_ID)

option(ENABLE_OMP "Enable the OpenMP support" ON)

if (ENABLE_OMP)
  set(USE_OMP 1)

  find_package (OpenMP REQUIRED)
  include_directories(BEFORE ${OpenMP_C_INCLUDE_DIRS})
endif(ENABLE_OMP)

option(ENABLE_MPI "Enable the MPI support" ON)

if (ENABLE_MPI)
  set(USE_MPI 1)

  find_package(MPI COMPONENTS CXX)
  include_directories(BEFORE ${MPI_INCLUDE_PATH})
endif(ENABLE_MPI)

option(ENABLE_LOGLEVEL_TRACE "Enable loglevel trace" OFF)

if (ENABLE_LOGLEVEL_TRACE)
  set(SUPPORT_LOGLEVEL_TRACE 1)
endif(ENABLE_LOGLEVEL_TRACE)

set(PostgreSQL_TYPE_INCLUDE_DIR "ignored")
find_package (PostgreSQL REQUIRED)
include_directories(BEFORE ${PostgreSQL_INCLUDE_DIR})

include_directories(BEFORE ${PROJECT_BINARY_DIR}/include)

add_subdirectory(dependencies)
add_subdirectory(src)
add_subdirectory(plugin)

message(STATUS "----------------------------------------------------------------

 Building EpiHiper 

   Source Dir             = ${CMAKE_SOURCE_DIR}
   Binary Dir             = ${CMAKE_BINARY_DIR}
   Install Prefix         = ${CMAKE_INSTALL_PREFIX}

  Options:

   Use Location Id        = ${ENABLE_LOCATION_ID}
   Use OpenMP             = ${ENABLE_OMP}
   Use MPI                = ${ENABLE_MPI}
   Log level trace        = ${ENABLE_LOGLEVEL_TRACE}
 Dependencies:

   MPI Libs               = ${MPI_LIBRARIES}
   MPI LDFLAGS            = ${MPI_LINKER_FLAGS}
   MPI include            = ${MPI_INCLUDE_PATH}

   OpenMP CXX Flags       = ${OpenMP_CXX_FLAGS}
   OpenMP Libs            = ${OpenMP_C_LIB_NAMES}
   OpenMP include         = ${OpenMP_C_INCLUDE_DIRS}
   
   PostgreSQL Libs        = ${PostgreSQL_LIBRARY}
   PostgreSQL include     = ${PostgreSQL_INCLUDE_DIR}
   
   libpqxx Libs           = ${CMAKE_BINARY_DIR}/lib/libpqxx.a
   libpqxx include        = ${CMAKE_BINARY_DIR}/include

   jansson Libs           = ${CMAKE_BINARY_DIR}/lib/libjansson.a
   jansson include        = ${CMAKE_BINARY_DIR}/include

----------------------------------------------------------------
")
