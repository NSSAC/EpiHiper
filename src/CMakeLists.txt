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

file(GLOB HEADERS_EPIHIPER *.h)
file(GLOB SOURCES_EPIHIPER *.cpp)

# create source group for IDEs
source_group(EpiHiper FILES ${SOURCES_EPIHIPER} ${HEADERS_EPIHIPER})

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})

include(actions/CMakeLists.txt)
include(diseaseModel/CMakeLists.txt)
include(intervention/CMakeLists.txt)
include(math/CMakeLists.txt)
include(network/CMakeLists.txt)
include(sets/CMakeLists.txt)
include(traits/CMakeLists.txt)
include(utilities/CMakeLists.txt)

add_executable (EpiHiper ${SOURCES_EPIHIPER})

add_dependencies(EpiHiper jansson)
target_link_libraries (EpiHiper ${MPI_LIBRARIES} ${CMAKE_BINARY_DIR}/lib/libjansson.a)
