# Copyright 2015-2024 Blue Brain Project/EPFL
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSDTI_SOURCES
  io/DTILoader.cpp
  io/DTISimulationHandler.cpp
  DTIPlugin.cpp
)

set(BRAYNSDTI_HEADERS
  io/DTILoader.h
  io/DTISimulationHandler.h
  DTIPlugin.h
)

add_library(braynsDTI SHARED ${BRAYNSDTI_SOURCES} ${BRAYNSDTI_HEADERS})

target_include_directories(braynsDTI PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(braynsDTI PRIVATE
  Brion Brain braynsCommon braynsParameters braynsPluginAPI braynsNetwork braynsEngine
)

set_target_properties(braynsDTI
    PROPERTIES
        VERSION ${BRAYNS_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsDTI
    EXPORT braynsDTI-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(EXPORT braynsDTI-targets
    DESTINATION share/braynsCircuitInfo/CMake
)
