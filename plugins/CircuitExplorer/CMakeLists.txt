# Copyright (c) 2015-2022, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

file(GLOB_RECURSE BRAYNSCIRCUITEXPLORER_SOURCES RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.cpp)
file(GLOB_RECURSE BRAYNSCIRCUITEXPLORER_HEADERS RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.h)

add_library(braynsCircuitExplorer SHARED ${BRAYNSCIRCUITEXPLORER_SOURCES} ${BRAYNSCIRCUITEXPLORER_HEADERS})

set(CIRCUITEXPLORER_INCLUDEDIR_LIST ${CMAKE_CURRENT_LIST_DIR})

target_include_directories(braynsCircuitExplorer PUBLIC
    "$<BUILD_INTERFACE:${CIRCUITEXPLORER_INCLUDEDIR_LIST}>"
    "$<INSTALL_INTERFACE:include>")

target_link_libraries(braynsCircuitExplorer PRIVATE
    braynsPluginAPI
    braynsNetwork
    sonata::sonata_shared
    MorphIO::morphio
    Brion
    Brain
    HighFive
    MVDTool
)

set_target_properties(braynsCircuitExplorer
    PROPERTIES
    VERSION ${BRAYNS_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

target_compile_options(braynsCircuitExplorer PRIVATE ${BRAYNS_COMPILE_OPTIONS})

# For Brion uint128_t
target_compile_options(braynsCircuitExplorer PRIVATE -Wno-deprecated-copy)

install(TARGETS braynsCircuitExplorer
    EXPORT braynsCircuitExplorer-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h")

install(EXPORT braynsCircuitExplorer-targets
    DESTINATION share/braynsCircuitExplorer/CMake
)
