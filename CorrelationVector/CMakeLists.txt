# CMakeList.txt : CMake project for Microsoft.CorrelationVector, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(LIBUUID_PATH "/usr/include/uuid/uuid.h")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

#Bring the headers into the project
include_directories(include)
 
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "src/*.cpp")

add_library (MicrosoftCorrelationVector SHARED ${SOURCES})

if(MSVC)
	set(WARNINGS "/WX" "/W4")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(WARNINGS "-Werror" "-Wall")
endif()

if(WIN32)
    target_compile_definitions(MicrosoftCorrelationVector PRIVATE GUID_WINDOWS)
else()
    if (NOT EXISTS LIBUUID_PATH)
        message(FATAL_ERROR "You need to run 'sudo apt-get install uuid-dev' to install uuid")
    endif()
    target_compile_definitions(MicrosoftCorrelationVector PRIVATE GUID_LIBUUID)
endif()

# Prepare "Catch" library for other executables
set(CATCH_INCLUDE_DIR "libs/catch")
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

# Make test executable
file(GLOB TEST_SOURCES "test/*.cpp")
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests Catch MicrosoftCorrelationVector)

include("libs/catch/ParseAndAddCatchTests.cmake")
ParseAndAddCatchTests(tests)