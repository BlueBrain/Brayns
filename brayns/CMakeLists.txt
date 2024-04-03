# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Authors: Cyrille Favreau <cyrille.favreau@epfl.ch>
# Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

configure_file(Version.h.in ${CMAKE_BINARY_DIR}/include/brayns/Version.h)

add_subdirectory(engine)
add_subdirectory(io)
add_subdirectory(json)
add_subdirectory(network)
add_subdirectory(parameters)
add_subdirectory(utils)

file(GLOB BRAYNS_HEADERS RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.h)
file(GLOB BRAYNS_SOURCES RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.cpp)

add_library(brayns SHARED ${BRAYNS_HEADERS} ${BRAYNS_SOURCES})

target_include_directories(brayns PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(brayns PUBLIC
    braynsEngine
    braynsNetwork
)

target_link_libraries(brayns PRIVATE
    braynsIO
    braynsUtils
)

target_compile_options(brayns PRIVATE ${BRAYNS_COMPILE_OPTIONS})

set_target_properties(brayns
    PROPERTIES
    VERSION ${BRAYNS_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS brayns
    EXPORT Brayns-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(FILES ${CMAKE_BINARY_DIR}/include/brayns/Version.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/brayns
)

install(FILES Brayns-config.cmake
    DESTINATION share/brayns/CMake
)

install(EXPORT Brayns-targets
    DESTINATION share/brayns/CMake
)
