# BEGIN: Copyright 
# MIT License 
#  
# Copyright (C) 2022 - 2023 Rector and Visitors of the University of Virginia 
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

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src)

file(GLOB SOURCES_EXAMPLE *.cpp)
file(GLOB HEADERS_EXAMPLE *.h)


add_library(EpiHiperPlugin SHARED ${SOURCES_EXAMPLE} ${HEADERS_EXAMPLE})
add_dependencies(EpiHiperPlugin jansson libpqxx spdlog)

if(APPLE)
  set_target_properties(EpiHiperPlugin PROPERTIES LINK_FLAGS "-undefined dynamic_lookup -flat_namespace")
endif(APPLE)