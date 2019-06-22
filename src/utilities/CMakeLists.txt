cmake_minimum_required (VERSION 3.11)

file(GLOB HEADERS_UTILITIES utilities/*.h)
file(GLOB SOURCES_UTILITIES utilities/*.cpp)

# create source group for IDEs
source_group(report FILES ${SOURCES_UTILITIES} ${HEADERS_UTILITIES})

# add files to main EPIHIPER sources
set(SOURCES_EPIHIPER ${SOURCES_EPIHIPER} ${SOURCES_UTILITIES})
set(HEADERS_EPIHIPER ${HEADERS_EPIHIPER} ${HEADERS_UTILITIES})
