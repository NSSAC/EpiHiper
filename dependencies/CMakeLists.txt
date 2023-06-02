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

  CONFIGURE_COMMAND cmake
    -DCMAKE_POLICY_DEFAULT_CMP0074=NEW
    -DPostgreSQL_TYPE_INCLUDE_DIR=ignored
    -DPostgreSQL_ROOT=${PostgreSQL_ROOT}
  	-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR} 
    -DCMAKE_INSTALL_LIBDIR=lib
    -DSKIP_BUILD_TEST=ON
  	${CMAKE_CURRENT_SOURCE_DIR}/libpqxx
  	
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
