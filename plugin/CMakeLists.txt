# BEGIN: Copyright 
# Copyright (C) 2022 Rector and Visitors of the University of Virginia 
# All rights reserved 
# END: Copyright 

# BEGIN: License 
# Licensed under the Apache License, Version 2.0 (the "License"); 
# you may not use this file except in compliance with the License. 
# You may obtain a copy of the License at 
#   http://www.apache.org/licenses/LICENSE-2.0 
# END: License 

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src)

file(GLOB SOURCES_EXAMPLE *.cpp)
file(GLOB HEADERS_EXAMPLE *.h)


add_library(EpiHiperPlugin SHARED ${SOURCES_EXAMPLE} ${HEADERS_EXAMPLE})
