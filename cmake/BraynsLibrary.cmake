# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

function(brayns_library TARGET)
    add_library(${TARGET})
    add_library(brayns::${TARGET} ALIAS ${TARGET})

    set(BRAYNS_SOURCE_DIRECTORY ${PROJECT_SOURCE_DIR}/src)
    set(TARGET_SOURCE_DIRECTORY ${BRAYNS_SOURCE_DIRECTORY}/brayns/${TARGET})

    file(GLOB_RECURSE HEADERS ${TARGET_SOURCE_DIRECTORY}/*.h)
    file(GLOB_RECURSE SOURCES ${TARGET_SOURCE_DIRECTORY}/*.cpp)

    target_sources(${TARGET} PRIVATE ${HEADERS} ${SOURCES})

    target_include_directories(
        ${TARGET}
        PUBLIC
        $<BUILD_INTERFACE:${BRAYNS_SOURCE_DIRECTORY}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    define_brayns_version_marco(${TARGET})

    install(
        TARGETS ${NAME}
        EXPORT BraynsTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    install(
        DIRECTORY ${TARGET_SOURCE_DIRECTORY}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/brayns
        FILES_MATCHING PATTERN *.h
    )
endfunction()
