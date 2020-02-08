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

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/jansson/CMakeLists.txt" OR
   NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libpqxx/CMakeLists.txt" OR
   NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/spdlog/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()


include(ExternalProject)

ExternalProject_Add(jansson
  PREFIX            ${CMAKE_BINARY_DIR}/dependencies
  SOURCE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}/jansson
  
  CONFIGURE_COMMAND  cmake 
  	-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR} 
  	-DJANSSON_BUILD_DOCS=OFF 
  	${CMAKE_CURRENT_SOURCE_DIR}/jansson
  	
  BUILD_COMMAND      make 
  INSTALL_COMMAND    make install
)

ExternalProject_Add(libpqxx
  PREFIX            ${CMAKE_BINARY_DIR}/dependencies
  SOURCE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}/libpqxx
  
  CONFIGURE_COMMAND  CXXFLAGS=-std=c++11 ${CMAKE_CURRENT_SOURCE_DIR}/libpqxx/configure 
  	--prefix=${CMAKE_BINARY_DIR} 
  	--libdir=${CMAKE_BINARY_DIR}/lib 
  	--with-postgres-include=${PGSQL_INCLUDE_DIR} 
  	--with-postgres-lib=${PGSQL_LIBRARY} 
  	--disable-documentation
  	
  BUILD_COMMAND      make 
  INSTALL_COMMAND    make install
)

ExternalProject_Add(spdlog
  PREFIX            ${CMAKE_BINARY_DIR}/dependencies
  SOURCE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}/spdlog

  PATCH_COMMAND     git checkout -- . && git apply ${CMAKE_CURRENT_SOURCE_DIR}/spdlog.patch
  CONFIGURE_COMMAND cmake
  	-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR} 
  	-DSPDLOG_BUILD_TESTS=OFF
  	-DSPDLOG_BUILD_EXAMPLE=OFF
  	${CMAKE_CURRENT_SOURCE_DIR}/spdlog
  	
  BUILD_COMMAND      make
  INSTALL_COMMAND    make install && cd ${CMAKE_CURRENT_SOURCE_DIR}/spdlog && git checkout -- .
)
