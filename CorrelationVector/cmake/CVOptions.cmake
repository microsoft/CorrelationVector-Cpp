# Define custom options for CMake.
# Options are passed to cmake through command line, -Doption=value
set (BUILD_TESTS
     OFF
     CACHE BOOL
           "Indicates if tests should be built.")

set (USE_STATIC_C_RUNTIME
     OFF
     CACHE BOOL
           "Indicates if static c runtime should be used.")

set (USE_STATIC_CXX_RUNTIME
     OFF
     CACHE BOOL
           "Indicates if static c++ runtime should be used.")

set (USE_STATIC_BOOST
    OFF
    CACHE BOOL
        "Link to static Boost libraries.")

set (USE_BOOST_UUID
    OFF
    CACHE BOOL
        "Use boost uuid as the guid implementation.")

set (CXX_COMPILE_OPTIONS
    ""
    CACHE
        STRING
        "Additional flags to use for c++ compile. If empty or not specified, default flags will be added."
    )

set (C_COMPILE_OPTIONS
    ""
    CACHE
        STRING
        "Additional flags to use for c compile. If empty or not specified, default flags will be added."
    )

set (CORRELATION_VECTOR_EXPORT_DIR
    correlation_vector
    CACHE
        STRING
        "Directory to install CMake config files."
    )

set (CORRELATION_VECTOR_INSTALL_HEADERS
    ON
    CACHE
        BOOL "Install header files."
    )

set (CORRELATION_VECTOR_INSTALL
    ON
    CACHE
        BOOL "Add install commands."
    )