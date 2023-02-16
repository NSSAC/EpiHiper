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

MACRO (FIND_OSHMEM)
ENDMACRO ()

# Check if we have cached results in case the last round was successful.
if (NOT (OSHMEM_INCLUDE_DIR AND OSHMEM_LIBRARIES) OR NOT OSHMEM_FOUND)

    set(OSHMEM_LDFLAGS)
    
	foreach(__FILE ${MPI_LIBRARIES})
	  get_filename_component(__PATH ${__FILE} DIRECTORY)
	  set(OSHMEM_LIB_PATH ${OSHMEM_LIB_PATH} ${__PATH})
	endforeach(__FILE)

	
    find_path(OSHMEM_INCLUDE_DIR mpp/shmem.h
	    PATHS ${MPI_INCLUDE_PATH})

    find_library(OSHMEM_LIBRARIES 
        NAMES oshmem
        PATHS ${OSHMEM_LIB_PATH})
    
    mark_as_advanced(OSHMEM_INCLUDE_DIR OSHMEM_LIBRARIES)

endif () # Check for cached values

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    oshmem
    REQUIRED_VARS OSHMEM_LIBRARIES OSHMEM_INCLUDE_DIR)
