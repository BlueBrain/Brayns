# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

file(GLOB_RECURSE BRAYNSMOLECULEEXPLORER_SOURCES RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.cpp)
file(GLOB_RECURSE BRAYNSMOLECULEEXPLORER_HEADERS RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.h)

add_library(braynsMoleculeExplorer SHARED ${BRAYNSMOLECULEEXPLORER_SOURCES} ${BRAYNSMOLECULEEXPLORER_HEADERS})

target_include_directories(braynsMoleculeExplorer PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>"
    "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(braynsMoleculeExplorer PRIVATE braynsPluginAPI braynsEngine braynsNetwork)

set_target_properties(braynsMoleculeExplorer PROPERTIES
    VERSION ${BRAYNS_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsMoleculeExplorer
    EXPORT braynsMoleculeExplorer-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(EXPORT braynsMoleculeExplorer-targets
    DESTINATION share/braynsMoleculeExplorer/CMake
)
