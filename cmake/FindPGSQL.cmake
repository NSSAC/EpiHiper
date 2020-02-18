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

# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
FindPGSQL
--------------

Find the PGSQL installation.

IMPORTED Targets
^^^^^^^^^^^^^^^^

This module defines :prop_tgt:`IMPORTED` target ``PGSQL::PGSQL``
if PGSQL has been found.

Result Variables
^^^^^^^^^^^^^^^^

This module will set the following variables in your project:

``PGSQL_FOUND``
  True if PGSQL is found.
``PGSQL_LIBRARIES``
  the PGSQL libraries needed for linking
``PGSQL_INCLUDE_DIRS``
  the directories of the PGSQL headers
``PGSQL_LIBRARY_DIRS``
  the link directories for PGSQL libraries
``PGSQL_VERSION_STRING``
  the version of PGSQL found
#]=======================================================================]

# ----------------------------------------------------------------------------
# History:
# This module is derived from the module originally found in the VTK source tree.
#
# ----------------------------------------------------------------------------
# Note:
# PGSQL_ADDITIONAL_VERSIONS is a variable that can be used to set the
# version number of the implementation of PGSQL.
# In Windows the default installation of PGSQL uses that as part of the path.
# E.g C:\Program Files\PostgreSQL\8.4.
# Currently, the following version numbers are known to this module:
# "11" "10" "9.6" "9.5" "9.4" "9.3" "9.2" "9.1" "9.0" "8.4" "8.3" "8.2" "8.1" "8.0"
#
# To use this variable just do something like this:
# set(PGSQL_ADDITIONAL_VERSIONS "9.2" "8.4.4")
# before calling find_package(PGSQL) in your CMakeLists.txt file.
# This will mean that the versions you set here will be found first in the order
# specified before the default ones are searched.
#
# ----------------------------------------------------------------------------
# You may need to manually set:
#  PGSQL_INCLUDE_DIR  - the path to where the PGSQL include files are.
#  PGSQL_LIBRARY_DIR  - The path to where the PGSQL library files are.
# If FindPGSQL.cmake cannot find the include files or the library files.
#
# ----------------------------------------------------------------------------
# The following variables are set if PGSQL is found:
#  PGSQL_FOUND         - Set to true when PGSQL is found.
#  PGSQL_INCLUDE_DIRS  - Include directories for PGSQL
#  PGSQL_LIBRARY_DIRS  - Link directories for PGSQL libraries
#  PGSQL_LIBRARIES     - The PGSQL libraries.
#
# The ``PGSQL::PGSQL`` imported target is also created.
#
# ----------------------------------------------------------------------------
# If you have installed PGSQL in a non-standard location.
# (Please note that in the following comments, it is assumed that <Your Path>
# points to the root directory of the include directory of PGSQL.)
# Then you have three options.
# 1) After CMake runs, set PGSQL_INCLUDE_DIR to <Your Path>/include and
#    PGSQL_LIBRARY_DIR to wherever the library pq (or libpq in windows) is
# 2) Use CMAKE_INCLUDE_PATH to set a path to <Your Path>/PGSQL<-version>. This will allow find_path()
#    to locate PGSQL_INCLUDE_DIR by utilizing the PATH_SUFFIXES option. e.g. In your CMakeLists.txt file
#    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "<Your Path>/include")
# 3) Set an environment variable called ${PGSQL_ROOT} that points to the root of where you have
#    installed PGSQL, e.g. <Your Path>.
#
# ----------------------------------------------------------------------------

set(PGSQL_INCLUDE_PATH_DESCRIPTION "top-level directory containing the PGSQL include directories. E.g /usr/local/include/PostgreSQL/8.4 or C:/Program Files/PostgreSQL/8.4/include")
set(PGSQL_INCLUDE_DIR_MESSAGE "Set the PGSQL_INCLUDE_DIR cmake cache entry to the ${PGSQL_INCLUDE_PATH_DESCRIPTION}")
set(PGSQL_LIBRARY_PATH_DESCRIPTION "top-level directory containing the PGSQL libraries.")
set(PGSQL_LIBRARY_DIR_MESSAGE "Set the PGSQL_LIBRARY_DIR cmake cache entry to the ${PGSQL_LIBRARY_PATH_DESCRIPTION}")
set(PGSQL_ROOT_DIR_MESSAGE "Set the PGSQL_ROOT system variable to where PGSQL is found on the machine E.g C:/Program Files/PostgreSQL/8.4")


set(PGSQL_KNOWN_VERSIONS ${PGSQL_ADDITIONAL_VERSIONS}
    "12" "11" "10" "9.6" "9.5" "9.4" "9.3" "9.2" "9.1" "9.0" "8.4" "8.3" "8.2" "8.1" "8.0")

# Define additional search paths for root directories.
set( PGSQL_ROOT_DIRECTORIES
   ENV PGSQL_ROOT
   ${PGSQL_ROOT}
)
foreach(suffix ${PGSQL_KNOWN_VERSIONS})
  if(WIN32)
    list(APPEND PGSQL_LIBRARY_ADDITIONAL_SEARCH_SUFFIXES
        "PostgreSQL/${suffix}/lib")
    list(APPEND PostgreSQL_INCLUDE_ADDITIONAL_SEARCH_SUFFIXES
        "PostgreSQL/${suffix}/include")
    list(APPEND PostgreSQL_TYPE_ADDITIONAL_SEARCH_SUFFIXES
        "PostgreSQL/${suffix}/include/server")
  endif()
  if(UNIX)
    list(APPEND PGSQL_LIBRARY_ADDITIONAL_SEARCH_SUFFIXES
        "pgsql-${suffix}/lib")
    list(APPEND PGSQL_INCLUDE_ADDITIONAL_SEARCH_SUFFIXES
        "pgsql-${suffix}/include")
    list(APPEND PGSQL_TYPE_ADDITIONAL_SEARCH_SUFFIXES
        "postgresql/${suffix}/server"
        "pgsql-${suffix}/include/server")
  endif()
endforeach()

#
# Look for an installation.
#
find_path(PGSQL_INCLUDE_DIR
  NAMES libpq-fe.h
  PATHS
   # Look in other places.
   ${PGSQL_ROOT_DIRECTORIES}
  PATH_SUFFIXES
    pgsql
    postgresql
    include
    ${PGSQL_INCLUDE_ADDITIONAL_SEARCH_SUFFIXES}
  # Help the user find it if we cannot.
  DOC "The ${PGSQL_INCLUDE_DIR_MESSAGE}"
)

find_path(PGSQL_TYPE_INCLUDE_DIR
  NAMES catalog/pg_type.h
  PATHS
   # Look in other places.
   ${PGSQL_ROOT_DIRECTORIES}
  PATH_SUFFIXES
    postgresql
    pgsql/server
    postgresql/server
    include/server
    ${PGSQL_TYPE_ADDITIONAL_SEARCH_SUFFIXES}
  # Help the user find it if we cannot.
  DOC "The ${PGSQL_INCLUDE_DIR_MESSAGE}"
)

# The PGSQL library.
set (PGSQL_LIBRARY_TO_FIND pq)
# Setting some more prefixes for the library
set (PGSQL_LIB_PREFIX "")
if ( WIN32 )
  set (PGSQL_LIB_PREFIX ${PGSQL_LIB_PREFIX} "lib")
  set (PGSQL_LIBRARY_TO_FIND ${PGSQL_LIB_PREFIX}${PGSQL_LIBRARY_TO_FIND})
endif()

find_library(PGSQL_LIBRARY
 NAMES ${PGSQL_LIBRARY_TO_FIND}
 PATHS
   ${PGSQL_ROOT_DIRECTORIES}
 PATH_SUFFIXES
   lib
   ${PGSQL_LIBRARY_ADDITIONAL_SEARCH_SUFFIXES}
 # Help the user find it if we cannot.
 DOC "The ${PGSQL_LIBRARY_DIR_MESSAGE}"
)
get_filename_component(PGSQL_LIBRARY_DIR ${PGSQL_LIBRARY} PATH)

if (PGSQL_INCLUDE_DIR)
  # Some platforms include multiple pg_config.hs for multi-lib configurations
  # This is a temporary workaround.  A better solution would be to compile
  # a dummy c file and extract the value of the symbol.
  file(GLOB _PG_CONFIG_HEADERS "${PGSQL_INCLUDE_DIR}/pg_config*.h")
  foreach(_PG_CONFIG_HEADER ${_PG_CONFIG_HEADERS})
    if(EXISTS "${_PG_CONFIG_HEADER}")
      file(STRINGS "${_PG_CONFIG_HEADER}" pgsql_version_str
           REGEX "^#define[\t ]+PG_VERSION_NUM[\t ]+.*")
      if(pgsql_version_str)
        string(REGEX REPLACE "^#define[\t ]+PG_VERSION_NUM[\t ]+([0-9]*).*"
               "\\1" _PGSQL_VERSION_NUM "${pgsql_version_str}")
        break()
      endif()
    endif()
  endforeach()
  if (_PGSQL_VERSION_NUM)
    math(EXPR _PGSQL_major_version "${_PGSQL_VERSION_NUM} / 10000")
    math(EXPR _PGSQL_minor_version "${_PGSQL_VERSION_NUM} % 10000")
    set(PGSQL_VERSION_STRING "${_PGSQL_major_version}.${_PGSQL_minor_version}")
    unset(_PGSQL_major_version)
    unset(_PGSQL_minor_version)
  else ()
    foreach(_PG_CONFIG_HEADER ${_PG_CONFIG_HEADERS})
      if(EXISTS "${_PG_CONFIG_HEADER}")
        file(STRINGS "${_PG_CONFIG_HEADER}" pgsql_version_str
             REGEX "^#define[\t ]+PG_VERSION[\t ]+\".*\"")
        if(pgsql_version_str)
          string(REGEX REPLACE "^#define[\t ]+PG_VERSION[\t ]+\"([^\"]*)\".*"
                 "\\1" PGSQL_VERSION_STRING "${pgsql_version_str}")
          break()
        endif()
      endif()
    endforeach()
  endif ()
  unset(_PGSQL_VERSION_NUM)
  unset(pgsql_version_str)
endif()

find_package_handle_standard_args(PGSQL
                                  REQUIRED_VARS PGSQL_LIBRARY PGSQL_INCLUDE_DIR
                                  VERSION_VAR PGSQL_VERSION_STRING)

# Now try to get the include and library path.
if(PGSQL_FOUND)
  if (NOT TARGET PGSQL::PGSQL)
    add_library(PGSQL::PGSQL UNKNOWN IMPORTED)
    set_target_properties(PGSQL::PGSQL PROPERTIES
      IMPORTED_LOCATION "${PGSQL_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${PGSQL_INCLUDE_DIR};${PGSQL_TYPE_INCLUDE_DIR}")
  endif ()
  set(PGSQL_INCLUDE_DIRS ${PGSQL_INCLUDE_DIR} ${PGSQL_TYPE_INCLUDE_DIR} )
  set(PGSQL_LIBRARY_DIRS ${PGSQL_LIBRARY_DIR} )
  set(PGSQL_LIBRARIES ${PGSQL_LIBRARY})
endif()

mark_as_advanced(PGSQL_INCLUDE_DIR PGSQL_TYPE_INCLUDE_DIR PGSQL_LIBRARY )
