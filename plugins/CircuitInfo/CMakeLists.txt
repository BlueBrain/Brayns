# Copyright 2015-2024 Blue Brain Project/EPFL
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>


set(BRAYNSCIRCUITINFO_HEADERS
  CircuitInfoPlugin.h
)

set(BRAYNSCIRCUITINFO_SOURCES
  CircuitInfoPlugin.cpp
)

add_library(braynsCircuitInfo SHARED ${BRAYNSCIRCUITINFO_HEADERS} ${BRAYNSCIRCUITINFO_SOURCES})

target_include_directories(braynsCircuitInfo PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(braynsCircuitInfo PRIVATE
  Brion Brain braynsCommon braynsParameters braynsPluginAPI braynsNetwork
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
