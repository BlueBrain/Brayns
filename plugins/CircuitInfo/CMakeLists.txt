# Copyright (c) 2015-2022, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

file(GLOB_RECURSE BRAYNSCIRCUITINFO_HEADERS RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.h)
file(GLOB_RECURSE BRAYNSCIRCUITINFO_SOURCES RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.cpp)

add_library(braynsCircuitInfo SHARED ${BRAYNSCIRCUITINFO_HEADERS} ${BRAYNSCIRCUITINFO_SOURCES})

target_include_directories(braynsCircuitInfo PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(braynsCircuitInfo PRIVATE
    Brion
    Brain
    braynsPluginAPI
    braynsNetwork
)

set_target_properties(braynsCircuitInfo
    PROPERTIES
    VERSION ${BRAYNS_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsCircuitInfo
    EXPORT braynsCircuitInfo-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(EXPORT braynsCircuitInfo-targets
    DESTINATION share/braynsCircuitInfo/CMake
)
