# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.7)

project(correlation_vector-root NONE)

include (CTest)
enable_testing()

add_subdirectory(CorrelationVector)