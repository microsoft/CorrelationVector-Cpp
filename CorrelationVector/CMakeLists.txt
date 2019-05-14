
cmake_minimum_required(VERSION 3.7)
project(correlation_vector CXX)

# Tell CMake to look in cmake for our custom CMake module files.
set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include (CVOptions)
include (CVHelpers)

add_global_definitions ()
set_global_compile_flags ()

set(CORRELATION_VECTOR_VERSION_MAJOR 1)
set(CORRELATION_VECTOR_VERSION_MINOR 0)
set(CORRELATION_VECTOR_VERSION_REVISION 0)

include(GNUInstallDirs)

# Reconfigure final output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# These settings can be used by the test targets
set(CorrelationVector_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(CorrelationVector_LIBRARY correlation_vector)
set(CorrelationVector_LIBRARIES correlation_vector)

add_subdirectory(src)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()