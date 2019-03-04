set(TARGETNAME cv_tests)
add_executable(${TARGETNAME} CorrelationVectorTests.cpp)

find_package(Catch2 REQUIRED)
target_link_libraries(${TARGETNAME} PRIVATE Catch2::Catch2 correlation_vector)
target_include_directories(${TARGETNAME} PRIVATE ../priv_include)

if (UNIX)
    find_package(Threads REQUIRED)
    target_link_libraries(${TARGETNAME} PRIVATE Threads::Threads)
endif()

include(Catch)
include(ParseAndAddCatchTests)
ParseAndAddCatchTests(${TARGETNAME})

# TODO: add tests for guid and spin_parameters