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

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})

include(actions/CMakeLists.txt)
include(db/CMakeLists.txt)
include(diseaseModel/CMakeLists.txt)
include(intervention/CMakeLists.txt)
include(math/CMakeLists.txt)
include(network/CMakeLists.txt)
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

add_library (EpiHiperLib-core OBJECT ${SOURCES_EPIHIPER} ${HEADERS_EPIHIPER})
add_dependencies(EpiHiperLib-core jansson libpqxx spdlog)

add_library (EpiHiperLib STATIC $<TARGET_OBJECTS:EpiHiperLib-core>)

add_executable (EpiHiper EpiHiper.cpp Simulation.cpp Simulation.h git-commit.h)
add_dependencies(EpiHiper EpiHiperLib GIT_COMMIT)
target_link_libraries (EpiHiper 
    EpiHiperLib
	${MPI_LIBRARIES}
	${PGSQL_LIBRARY} 
	${CMAKE_BINARY_DIR}/lib/libjansson.a 
	${CMAKE_BINARY_DIR}/lib/libpqxx.a
)

add_executable (EpiHiperPartition Partition.cpp)
add_dependencies(EpiHiperPartition EpiHiperLib GIT_COMMIT)
target_link_libraries (EpiHiperPartition 
    EpiHiperLib
	${MPI_LIBRARIES}
	${PGSQL_LIBRARY} 
	${CMAKE_BINARY_DIR}/lib/libjansson.a 
	${CMAKE_BINARY_DIR}/lib/libpqxx.a
)

# add_executable (rma rma-new.cpp)
# add_dependencies(rma EpiHiperLib)
# target_link_libraries (rma 
#     EpiHiperLib
# 	${MPI_LIBRARIES}
# 	${PGSQL_LIBRARY} 
# 	${CMAKE_BINARY_DIR}/lib/libjansson.a 
# 	${CMAKE_BINARY_DIR}/lib/libpqxx.a
# )

