set(TARGETNAME correlation_vector)
add_library(${TARGETNAME} correlation_vector.cpp guid.cpp)

target_include_directories(${TARGETNAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

if (USE_BOOST_UUID)
    find_boost_components()
    target_compile_definitions(${TARGETNAME} PUBLIC GUID_BOOST)
    target_link_libraries(${TARGETNAME} PUBLIC Boost::boost)
else()
    if (WIN32)
        target_compile_definitions(${TARGETNAME} PUBLIC GUID_WINDOWS)
    elseif (UNIX)
        # apt-get install pkg-config uuid-dev
        find_package(PkgConfig REQUIRED)
        # TODO: move to FindUUID module
        pkg_check_modules(UUID uuid)
        if (UUID_FOUND)
            message("Found and using uuid.")
            target_include_directories(${TARGETNAME} PUBLIC ${UUID_INCLUDE_DIRS})
            target_link_libraries(${TARGETNAME} PRIVATE ${UUID_LIBRARIES})
            target_compile_definitions(${TARGETNAME} PUBLIC GUID_LIBUUID)
        endif()
    endif()
endif()

target_compile_features(${TARGETNAME} PUBLIC cxx_std_11)

set(HEADERS_CORRELATION_VECTOR
    ../include/correlation_vector/correlation_vector.h
    ../include/correlation_vector/guid.h
    ../include/correlation_vector/spin_parameters.h)

if(CORRELATION_VECTOR_INSTALL_HEADERS)
    install(FILES ${HEADERS_CORRELATION_VECTOR} DESTINATION include/correlation_vector)
endif()

if(CORRELATION_VECTOR_INSTALL)
    set(CORRELATION_VECTOR_TARGETS correlation_vector)
    install(
        TARGETS ${CORRELATION_VECTOR_TARGETS}
        EXPORT correlation_vector-targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    # Install PDB files produced by MSVC.
    if (MSVC)
        if (BUILD_SHARED_LIBS)
            set(CV_PDB_FILES $<TARGET_PDB_FILE:${TARGETNAME}>)
        else ()
            if (NOT CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY)
                set_target_properties(${TARGETNAME} PROPERTIES
                    COMPILE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
                )
            endif()

            set_target_properties(${TARGETNAME} PROPERTIES
                COMPILE_PDB_NAME "${TARGETNAME}")

            get_target_property(CV_PDB_DIR ${TARGETNAME} COMPILE_PDB_OUTPUT_DIRECTORY)
            set(CV_PDB_FILES
                ${CV_PDB_DIR}/${TARGETNAME}.pdb
            )
        endif ()

        install(
                FILES ${CV_PDB_FILES}
                DESTINATION ${CMAKE_INSTALL_LIBDIR}
                CONFIGURATIONS "Debug"
                OPTIONAL
        )
    endif ()

    configure_file(../cmake/correlation_vector-config.in.cmake "${CMAKE_CURRENT_BINARY_DIR}/correlation_vector-config.cmake" @ONLY)

    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/correlation_vector-config.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/${CORRELATION_VECTOR_EXPORT_DIR}
    )
    install(
        EXPORT correlation_vector-targets
        FILE correlation_vector-targets.cmake
        NAMESPACE microsoft::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/${CORRELATION_VECTOR_EXPORT_DIR}
    )
endif()