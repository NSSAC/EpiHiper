file(GLOB CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB H_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

set(TEST_FILES ${CPP_FILES} ${C_FILES} ${H_FILES})

include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable(EpiHiperTest ${TEST_FILES})
add_dependencies(EpiHiperTest EpiHiperLib)

set_target_properties(EpiHiperTest PROPERTIES
    CXX_STANDARD ${CMAKE_CXX_STANDARD}
    CXX_STANDARD_REQUIRED ${CMAKE_CXX_STANDARD_REQUIRED}
    CXX_EXTENSIONS ${CMAKE_CXX_EXTENSIONS}
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(EpiHiperTest PRIVATE -Werror -Wall -Wextra -Wformat=0 -Wno-c++20-compat ${OpenMP_CXX_FLAGS})
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(EpiHiperTest PRIVATE -Wall -Wextra -Wno-cast-function-type -Wformat=0 ${OpenMP_CXX_FLAGS})
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

if (ENABLE_OMP)
	set (EPIHIPER_LIBARIES 
		EpiHiperLib
		${CMAKE_BINARY_DIR}/lib/libjansson.a 
		${CMAKE_BINARY_DIR}/lib/libpqxx.a
		${MPI_LIBRARIES}
		${PostgreSQL_LIBRARY} 
		OpenMP::OpenMP_CXX
		-lpthread
		-ldl
	)
else ()
	set (EPIHIPER_LIBARIES 
		EpiHiperLib
		${CMAKE_BINARY_DIR}/lib/libjansson.a 
		${CMAKE_BINARY_DIR}/lib/libpqxx.a
		${MPI_LIBRARIES}
		${PostgreSQL_LIBRARY} 
		-lpthread
		-ldl
	)

	target_compile_options(EpiHiperTest BEFORE PRIVATE -Wno-unknown-pragmas)
endif (ENABLE_OMP)

target_link_libraries (EpiHiperTest PUBLIC ${EPIHIPER_LIBARIES})

add_test(NAME EpiHiperTestRun COMMAND "$<TARGET_FILE:EpiHiperTest>")
set_tests_properties(EpiHiperTestRun PROPERTIES ENVIRONMENT "srcdir=${PROJECT_SOURCE_DIR};OMP_NUM_THREADS=1")