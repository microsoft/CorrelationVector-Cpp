# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.7)

project("Microsoft.CorrelationVector")

# Enable testing
enable_testing()

# Include sub-projects.
add_subdirectory("CorrelationVector")