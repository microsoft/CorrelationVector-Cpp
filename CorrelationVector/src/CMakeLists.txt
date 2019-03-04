set(TARGETNAME correlation_vector)
add_library(${TARGETNAME} correlation_vector.cpp guid.cpp)

target_include_directories(${TARGETNAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    PRIVATE
        ../priv_include)

if(WIN32)
    target_compile_definitions(${TARGETNAME} PUBLIC GUID_WINDOWS)
elseif(UNIX)
    # apt-get install pkg-config uuid-dev
    find_package(PkgConfig REQUIRED)
    # TODO: move to FindUUID module
    pkg_check_modules(UUID uuid)
    if(UUID_FOUND)
        message("Found and using uuid.")
        target_include_directories(${TARGETNAME} PUBLIC ${UUID_INCLUDE_DIRS})
        target_link_libraries(${TARGETNAME} PRIVATE ${UUID_LIBRARIES})
        target_compile_definitions(${TARGETNAME} PUBLIC GUID_LIBUUID)
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
    message("The cmake install lib dir: ${CMAKE_INSTALL_LIBDIR}")
    install(
        TARGETS ${CORRELATION_VECTOR_TARGETS}
        EXPORT correlation_vector-targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    configure_file(../cmake/correlation_vector-config.in.cmake "${CMAKE_CURRENT_BINARY_DIR}/correlation_vector-config.cmake" @ONLY)

    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/correlation_vector-config.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/${CORRELATION_VECTOR_EXPORT_DIR}
    )
    install(
        EXPORT correlation_vector-targets
        FILE correlation_vector-targets.cmake
        NAMESPACE telemetry::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/${CORRELATION_VECTOR_EXPORT_DIR}
    )
endif()