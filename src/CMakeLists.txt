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

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})

include(actions/CMakeLists.txt)
include(db/CMakeLists.txt)
include(diseaseModel/CMakeLists.txt)
include(intervention/CMakeLists.txt)
include(math/CMakeLists.txt)
include(network/CMakeLists.txt)
include(plugins/CMakeLists.txt)
include(sets/CMakeLists.txt)
include(traits/CMakeLists.txt)
include(utilities/CMakeLists.txt)
include(variables/CMakeLists.txt)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
  add_custom_target(GIT_COMMIT 
  	COMMAND echo "\\#define GIT_COMMIT" \\\"`${GIT_EXECUTABLE} describe --always --dirty`\\\" > ${CMAKE_CURRENT_BINARY_DIR}/git-commit.h
    COMMAND if [ ! -e git-commit.h ] || ! diff --brief git-commit.h ${CMAKE_CURRENT_BINARY_DIR}/git-commit.h &> /dev/null\; then cp ${CMAKE_CURRENT_BINARY_DIR}/git-commit.h git-commit.h\; fi
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
else()
  add_custom_target(GIT_COMMIT 
  	COMMAND echo "\\#define GIT_COMMIT" \\\"unknown\\\" > ${CMAKE_CURRENT_BINARY_DIR}/git-commit.h
    COMMAND if [ ! -e git-commit.h ] || ! diff --brief git-commit.h ${CMAKE_CURRENT_BINARY_DIR}/git-commit.h &> /dev/null\; then cp ${CMAKE_CURRENT_BINARY_DIR}/git-commit.h git-commit.h\; fi
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/EpiHiperConfig.h.in ${CMAKE_CURRENT_SOURCE_DIR}/EpiHiperConfig.h)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library (EpiHiperLib-core OBJECT ${SOURCES_EPIHIPER} ${HEADERS_EPIHIPER})
add_dependencies(EpiHiperLib-core jansson libpqxx spdlog)
set_property(TARGET EpiHiperLib-core PROPERTY CXX_STANDARD 11)
set_property(TARGET EpiHiperLib-core PROPERTY CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(EpiHiperLib-core PRIVATE -Werror -Wall -Wextra -Wno-pmf-conversions ${OpenMP_CXX_FLAGS} -lpthread)
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(EpiHiperLib-core PRIVATE -Wall -Wextra -Wsuggest-override ${OpenMP_CXX_FLAGS} -lpthread)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

add_library (EpiHiperLib SHARED $<TARGET_OBJECTS:EpiHiperLib-core>)
add_dependencies(EpiHiperLib jansson libpqxx spdlog)

if(APPLE)
  set_target_properties(EpiHiperLib PROPERTIES LINK_FLAGS "-undefined dynamic_lookup -flat_namespace")
endif(APPLE)

add_executable (EpiHiper EpiHiper.cpp EpiHiperConfig.h)
add_dependencies(EpiHiper EpiHiperLib GIT_COMMIT)

if (ENABLE_OMP)
	set (EPIHIPER_LIBARIES 
		EpiHiperLib
		${MPI_LIBRARIES}
		${PGSQL_LIBRARY} 
		${CMAKE_BINARY_DIR}/lib/libjansson.a 
		${CMAKE_BINARY_DIR}/lib/libpqxx.a
		OpenMP::OpenMP_CXX
		-lpthread
		-ldl
	)
else ()
	set (EPIHIPER_LIBARIES 
		EpiHiperLib
		${MPI_LIBRARIES}
		${PGSQL_LIBRARY} 
		${CMAKE_BINARY_DIR}/lib/libjansson.a 
		${CMAKE_BINARY_DIR}/lib/libpqxx.a
		-lpthread
		-ldl
	)

	target_compile_options(EpiHiperLib-core BEFORE PRIVATE -Wno-unknown-pragmas)
endif (ENABLE_OMP)

target_link_libraries (EpiHiper PUBLIC ${EPIHIPER_LIBARIES})

add_executable (EpiHiperPartition Partition.cpp EpiHiperConfig.h)
add_dependencies(EpiHiperPartition EpiHiperLib GIT_COMMIT)
target_link_libraries (EpiHiperPartition PUBLIC ${EPIHIPER_LIBARIES})

add_executable (EpiHiperR0 EpiHiperR0.cpp EpiHiperConfig.h)
add_dependencies(EpiHiperR0 EpiHiperLib GIT_COMMIT)
target_link_libraries (EpiHiperR0 PUBLIC ${EPIHIPER_LIBARIES})

add_executable (EpiHiperModelAnalyzer EpiHiperModelAnalyzer.cpp EpiHiperConfig.h)
add_dependencies(EpiHiperModelAnalyzer EpiHiperLib GIT_COMMIT)
target_link_libraries (EpiHiperModelAnalyzer PUBLIC ${EPIHIPER_LIBARIES})
