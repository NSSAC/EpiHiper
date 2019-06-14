cmake_minimum_required (VERSION 2.6)
project (EpiHiper)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")
set(EpiHiper_SRCS EpiHiper.cpp Misc.cpp SimConfig.cpp Simulation.cpp)

include_directories("${PROJECT_BINARY_DIR}")
add_executable (EpiHiper ${EpiHiper_SRCS})
find_package (MPI REQUIRED)
if (MPI_FOUND)
  include_directories (SYSTEM ${MPI_INCLUDE_PATH})
  target_link_libraries (EpiHiper ${MPI_LIBRARIES})
endif (MPI_FOUND)
