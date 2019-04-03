include (CVOptions)

macro (configure_c_runtime)
    if (MSVC)
        set (STATIC_C_RUNTIME_FLAG "/MT")
        set (STATIC_C_RUNTIME_FLAG_MATCH "/MT")
        set (DYNAMIC_C_RUNTIME_FLAG "/MD")
        set (DYNAMIC_C_RUNTIME_FLAG_MATCH "/MD")
    elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
        # TODO: Need to figure this out for clang
        set (STATIC_C_RUNTIME_FLAG "")
        set (STATIC_C_RUNTIME_FLAG_MATCH "NOMATCH")
        set (DYNAMIC_C_RUNTIME_FLAG "")
        set (DYNAMIC_C_RUNTIME_FLAG_MATCH "NOMATCH")
    else ()
        set (STATIC_C_RUNTIME_FLAG "-static-libgcc")
        set (STATIC_C_RUNTIME_FLAG_MATCH "\\\-static\\\-libgcc")
        set (DYNAMIC_C_RUNTIME_FLAG "-shared-libgcc")
        set (DYNAMIC_C_RUNTIME_FLAG_MATCH "\\\-shared\\\-libgcc")
    endif ()

    if (USE_STATIC_C_RUNTIME)
        set (DESIRED_C_RUNTIME_FLAG ${STATIC_C_RUNTIME_FLAG})
        set (REPLACE_C_RUNTIME_FLAG ${DYNAMIC_C_RUNTIME_FLAG_MATCH})
    else ()
        set (DESIRED_C_RUNTIME_FLAG ${DYNAMIC_C_RUNTIME_FLAG})
        set (REPLACE_C_RUNTIME_FLAG ${STATIC_C_RUNTIME_FLAG_MATCH})
    endif ()

    set (c_variables
         CMAKE_C_FLAGS_DEBUG
         CMAKE_C_FLAGS_MINSIZEREL
         CMAKE_C_FLAGS_RELEASE
         CMAKE_C_FLAGS_RELWITHDEBINFO)

    # Replace the c compiler options
    foreach (variable ${c_variables})
        if (${variable} MATCHES "${REPLACE_C_RUNTIME_FLAG}")
            string (REGEX
                    REPLACE ${REPLACE_C_RUNTIME_FLAG}
                            ${DESIRED_C_RUNTIME_FLAG}
                            ${variable}
                            "${${variable}}")
        else ()
            set (${variable} "${${variable}} ${DESIRED_C_RUNTIME_FLAG}")
        endif ()
    endforeach ()
endmacro (configure_c_runtime)

macro (configure_cxx_runtime)
    if (MSVC)
        set (STATIC_CXX_RUNTIME_FLAG "/MT")
        set (STATIC_CXX_RUNTIME_FLAG_MATCH "/MT")
        set (DYNAMIC_CXX_RUNTIME_FLAG "/MD")
        set (DYNAMIC_CXX_RUNTIME_FLAG_MATCH "/MD")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        # TODO: Need to figure this out for clang
        set (STATIC_CXX_RUNTIME_FLAG "")
        set (STATIC_CXX_RUNTIME_FLAG_MATCH "NOMATCH")
        set (DYNAMIC_CXX_RUNTIME_FLAG "")
        set (DYNAMIC_CXX_RUNTIME_FLAG_MATCH "NOMATCH")
    else ()
        set (STATIC_CXX_RUNTIME_FLAG "-static-libstdc++")
        set (STATIC_CXX_RUNTIME_FLAG_MATCH "\\\-static\\\-libstdc\\\+\\\+")
        set (DYNAMIC_CXX_RUNTIME_FLAG "-lstdc++")
        set (DYNAMIC_CXX_RUNTIME_FLAG_MATCH "\\\-lstdc\\\+\\\+")
    endif ()

    if (USE_STATIC_CXX_RUNTIME)
        set (DESIRED_CXX_RUNTIME_FLAG ${STATIC_CXX_RUNTIME_FLAG})
        set (REPLACE_CXX_RUNTIME_FLAG ${DYNAMIC_CXX_RUNTIME_FLAG_MATCH})
    else ()
        set (DESIRED_CXX_RUNTIME_FLAG ${DYNAMIC_CXX_RUNTIME_FLAG})
        set (REPLACE_CXX_RUNTIME_FLAG ${STATIC_CXX_RUNTIME_FLAG_MATCH})
    endif ()

    set (cxx_variables
         CMAKE_CXX_FLAGS_DEBUG
         CMAKE_CXX_FLAGS_MINSIZEREL
         CMAKE_CXX_FLAGS_RELEASE
         CMAKE_CXX_FLAGS_RELWITHDEBINFO)

    # Replace the cxx compiler options
    foreach (variable ${cxx_variables})
        if (${variable} MATCHES "${REPLACE_CXX_RUNTIME_FLAG}")
            string (REGEX
                    REPLACE ${REPLACE_CXX_RUNTIME_FLAG}
                            ${DESIRED_CXX_RUNTIME_FLAG}
                            ${variable}
                            "${${variable}}")
        else ()
            set (${variable} "${${variable}} ${DESIRED_CXX_RUNTIME_FLAG}")
        endif ()
    endforeach ()
endmacro ()

macro (find_boost_components)
    # set (Boost_DEBUG ON)
    set (Boost_FIND_QUIETLY ON)
    # TODO: Lower the required boost version if possible
    find_package (Boost 1.43 REQUIRED COMPONENTS ${ARGV})
endmacro (find_boost_components)

macro (add_global_boost_definitions)
    # Note: When using vcpkg, Boost_USE_STATIC_LIBS is ignored.
    set (Boost_USE_STATIC_LIBS ${USE_STATIC_BOOST})
endmacro (add_global_boost_definitions)

macro (add_global_win_definitions)
    add_definitions ("-DWIN32 -D_WIN32 -DUNICODE" "-D_UNICODE")
endmacro (add_global_win_definitions)

macro (add_global_definitions)
    if (WIN32)
        add_global_win_definitions ()
    endif ()

    add_global_boost_definitions ()
endmacro (add_global_definitions)

macro (set_global_compile_flags)
    if (MSVC)
        set (MANDATORY_C_FLAGS "")
        set (DEFAULT_C_FLAGS "")
        set (DEFAULT_C_DEBUG_FLAGS "")
        set (DEFAULT_C_RELEASE_FLAGS "")

        set (MANDATORY_CXX_FLAGS "/EHsc")
        set (DEFAULT_CXX_FLAGS "/MP /permissive- /sdl /W3 /WX")
        set (DEFAULT_CXX_DEBUG_FLAGS "/Od /bigobj /Zi")
        # O2 - optimize for speed
        set (DEFAULT_CXX_RELEASE_FLAGS "/O2")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set (MANDATORY_C_FLAGS "")
        set (DEFAULT_C_FLAGS "")
        set (DEFAULT_C_DEBUG_FLAGS "")
        set (DEFAULT_C_RELEASE_FLAGS "")

        set (MANDATORY_CXX_FLAGS "")
        set (DEFAULT_CXX_FLAGS "-Wall -Werror -pedantic")
        set (DEFAULT_CXX_DEBUG_FLAGS "-g3 -O0")
        set (DEFAULT_CXX_RELEASE_FLAGS "-O3")
    else ()
        set (MANDATORY_C_FLAGS "")
        set (DEFAULT_C_FLAGS "")
        set (DEFAULT_C_DEBUG_FLAGS "")
        set (DEFAULT_C_RELEASE_FLAGS "")

        set (MANDATORY_CXX_FLAGS "")
        set (DEFAULT_CXX_FLAGS "-Wall -Werror -pedantic")
        set (DEFAULT_CXX_DEBUG_FLAGS "-g3 -Og")
        set (DEFAULT_CXX_RELEASE_FLAGS "-O3")
    endif ()

    # Overrides were specified
    if (CXX_COMPILE_OPTIONS)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MANDATORY_CXX_FLAGS} ${CXX_COMPILE_OPTIONS}")
    else ()
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MANDATORY_CXX_FLAGS} ${DEFAULT_CXX_FLAGS}")
        set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEFAULT_CXX_DEBUG_FLAGS}")
        set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${DEFAULT_CXX_RELEASE_FLAGS}")
    endif ()

    if (C_COMPILE_OPTIONS)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MANDATORY_C_FLAGS} ${C_COMPILE_OPTIONS}")
    else ()
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MANDATORY_C_FLAGS} ${DEFAULT_C_FLAGS}")
        set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${DEFAULT_C_DEBUG_FLAGS}")
        set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${DEFAULT_C_RELEASE_FLAGS}")
    endif ()

    configure_c_runtime ()
    configure_cxx_runtime ()
endmacro (set_global_compile_flags)
