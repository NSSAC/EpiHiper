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

file(GLOB HEADERS_VARIABLE variable/*.h)
file(GLOB SOURCES_VARIABLE variable/*.cpp)

# create source group for IDEs
source_group(variable FILES ${SOURCES_VARIABLE} ${HEADERS_VARIABLE})

# add files to main EPIHIPER sources
set(SOURCES_EPIHIPER ${SOURCES_EPIHIPER} ${SOURCES_VARIABLE})
set(HEADERS_EPIHIPER ${HEADERS_EPIHIPER} ${HEADERS_VARIABLE})
