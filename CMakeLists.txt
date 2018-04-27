# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project("Microsoft.CorrelationVector" VERSION 1.0 DESCRIPTION "Implementation of the CorrelationVector protocol for tracing and correlation of events through a distributed system")

# Enable testing
enable_testing()

# Include sub-projects.
add_subdirectory("CorrelationVector")