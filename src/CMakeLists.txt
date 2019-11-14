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

file(GLOB _HEADERS_EPIHIPER *.h)
file(GLOB _SOURCES_EPIHIPER *.cpp)
list(REMOVE_ITEM _SOURCES_EPIHIPER ${CMAKE_CURRENT_SOURCE_DIR}/rma.cpp)

# create source group for IDEs
source_group(EpiHiper FILES ${_SOURCES_EPIHIPER} ${_HEADERS_EPIHIPER})

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})

include(actions/CMakeLists.txt)
include(db/CMakeLists.txt)
include(diseaseModel/CMakeLists.txt)
include(initialization/CMakeLists.txt)
include(math/CMakeLists.txt)
include(network/CMakeLists.txt)
include(sets/CMakeLists.txt)
include(traits/CMakeLists.txt)
include(utilities/CMakeLists.txt)
include(variables/CMakeLists.txt)

add_executable (EpiHiper ${_SOURCES_EPIHIPER} ${SOURCES_EPIHIPER})

add_dependencies(EpiHiper jansson libpqxx)

target_link_libraries (EpiHiper 
	${MPI_LIBRARIES}
    ${OSHMEM_LIBRARIES}
	${PGSQL_LIBRARY} 
	${CMAKE_BINARY_DIR}/lib/libjansson.a 
	${CMAKE_BINARY_DIR}/lib/libpqxx.a
)


add_executable (rma ${CMAKE_CURRENT_SOURCE_DIR}/rma.cpp ${SOURCES_EPIHIPER})

add_dependencies(rma jansson libpqxx)

target_link_libraries (rma 
    ${MPI_LIBRARIES}
    ${OSHMEM_LIBRARIES}
    ${PGSQL_LIBRARY} 
    ${CMAKE_BINARY_DIR}/lib/libjansson.a 
    ${CMAKE_BINARY_DIR}/lib/libpqxx.a
    /usr/lib64/libpthread.so
)
