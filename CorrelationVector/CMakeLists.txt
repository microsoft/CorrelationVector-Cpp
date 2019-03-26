
cmake_minimum_required(VERSION 3.7)
project(correlation_vector CXX)

set(CORRELATION_VECTOR_VERSION_MAJOR 1)
set(CORRELATION_VECTOR_VERSION_MINOR 0)
set(CORRELATION_VECTOR_VERSION_REVISION 0)

# Set Options and Defaults
set(CORRELATION_VECTOR_EXPORT_DIR correlation_vector CACHE STRING "Directory to install CMake config files.")
set(CORRELATION_VECTOR_INSTALL_HEADERS ON CACHE BOOL "Install header files.")
set(CORRELATION_VECTOR_INSTALL ON CACHE BOOL "Add install commands.")

include(GNUInstallDirs)

# Reconfigure final output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE -DWIN32)
endif()

# These settings can be used by the test targets
set(CorrelationVector_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(CorrelationVector_LIBRARY correlation_vector)
set(CorrelationVector_LIBRARIES correlation_vector)

add_subdirectory(src)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()