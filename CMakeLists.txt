# BEGIN: Copyright 
# Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
# All rights reserved 
# END: Copyright 

# BEGIN: License 
# Licensed under the Apache License, Version 2.0 (the "License"); 
# you may not use this file except in compliance with the License. 
# You may obtain a copy of the License at 
#   http://www.apache.org/licenses/LICENSE-2.0 
# END: License 

cmake_minimum_required (VERSION 3.11)
project (EpiHiper VERSION 1.2.1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)
set(CMAKE_CXX_STANDARD 11)

option(ENABLE_LOCATION_ID "Enable the Location Id field in the network" OFF)

if (ENABLE_LOCATION_ID)
  set(USE_LOCATION_ID 1)
endif(ENABLE_LOCATION_ID)

option(ENABLE_OMP "Enable the OpenMP support" ON)

if (ENABLE_OMP)
  set(USE_OMP 1)

  find_package (OpenMP REQUIRED)
  include_directories(BEFORE ${OpenMP_INCLUDE_PATH})
endif(ENABLE_OMP)

option(ENABLE_MPI "Enable the MPI support" ON)

if (ENABLE_MPI)
  set(USE_MPI 1)

  find_package (MPI REQUIRED)
  include_directories(BEFORE ${MPI_INCLUDE_PATH})
endif(ENABLE_MPI)

find_package (PGSQL REQUIRED)
include_directories(BEFORE ${PGSQL_INCLUDE_DIR})

include_directories(BEFORE ${PROJECT_BINARY_DIR}/include)

add_subdirectory(dependencies)
add_subdirectory(src)

message(STATUS "-----------------------------------------------------------

 Building EpiHiper 

   Source Dir         = ${CMAKE_SOURCE_DIR}
   Binary Dir         = ${CMAKE_BINARY_DIR}
   Install Prefix     = ${CMAKE_INSTALL_PREFIX}

  Options:

   Use Location Id    = ${ENABLE_LOCATION_ID}
   Use OpenMP         = ${ENABLE_OMP}
   Use MPI            = ${ENABLE_MPI}

 Dependencies:

   MPI Libs           = ${MPI_LIBRARIES}
   MPI LDFLAGS        = ${MPI_LINKER_FLAGS}
   MPI include        = ${MPI_INCLUDE_PATH}

   OpenMP CXX Flags   = ${OpenMP_CXX_FLAGS}
   OpenMP Libs        = ${OpenMP_C_LIB_NAMES}
   
   PostgreSQL Libs    = ${PGSQL_LIBRARY}
   PostgreSQL include = ${PGSQL_INCLUDE_DIR}
   
   libpqxx Libs       = ${CMAKE_BINARY_DIR}/lib/libpqxx.a
   libpqxx include    = ${CMAKE_BINARY_DIR}/include

   jansson Libs       = ${CMAKE_BINARY_DIR}/lib/libjansson.a
   jansson include    = ${CMAKE_BINARY_DIR}/include

-----------------------------------------------------------
")
