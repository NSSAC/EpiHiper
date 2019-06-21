cmake_minimum_required (VERSION 3.11)

include(ExternalProject)

ExternalProject_Add(jansson
  PREFIX            ${CMAKE_BINARY_DIR}/dependencies
  SOURCE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}/jansson
  
  CONFIGURE_COMMAND  cmake -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/jansson
  BUILD_COMMAND      make 
  INSTALL_COMMAND    make install
)
