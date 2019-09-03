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

# Copyright (C) 2017 - 2018 by Pedro Mendes, Virginia Tech Intellectual 
# Properties, Inc., University of Heidelberg, and University of 
# of Connecticut School of Medicine. 
# All rights reserved. 

# Copyright (C) 2013 - 2016 by Pedro Mendes, Virginia Tech Intellectual 
# Properties, Inc., University of Heidelberg, and The University 
# of Manchester. 
# All rights reserved. 

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
