cmake_minimum_required (VERSION 3.11)

file(GLOB HEADERS_TRAITS traits/*.h)
file(GLOB SOURCES_TRAITS traits/*.cpp)

# create source group for IDEs
source_group(report FILES ${SOURCES_TRAITS} ${HEADERS_TRAITS})

# add files to main EPIHIPER sources
set(SOURCES_EPIHIPER ${SOURCES_EPIHIPER} ${SOURCES_TRAITS})
set(HEADERS_EPIHIPER ${HEADERS_EPIHIPER} ${HEADERS_TRAITS})
