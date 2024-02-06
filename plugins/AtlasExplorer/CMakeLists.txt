# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

file(GLOB_RECURSE BRAYNSATLASEXPLORER_SOURCES RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.cpp)
file(GLOB_RECURSE BRAYNSATLASEXPLORER_HEADERS RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.h)

add_library(braynsAtlasExplorer SHARED ${BRAYNSATLASEXPLORER_SOURCES} ${BRAYNSATLASEXPLORER_HEADERS})

target_include_directories(braynsAtlasExplorer PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>"
    "$<INSTALL_INTERFACE:include>")
target_link_libraries(braynsAtlasExplorer PRIVATE braynsPluginAPI braynsNetwork ${ZLIB_LIBRARIES} ${BZIP2_LIBRARIES})
target_include_directories(braynsAtlasExplorer PRIVATE ${ZLIB_INCLUDE_DIRS} ${BZIP2_INCLUDE_DIRS})

target_compile_options(braynsAtlasExplorer PRIVATE ${BRAYNS_COMPILE_OPTIONS})

set_target_properties(braynsAtlasExplorer
    PROPERTIES
    VERSION ${BRAYNS_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsAtlasExplorer
    EXPORT braynsAtlasExplorer-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(EXPORT braynsAtlasExplorer-targets
    DESTINATION share/braynsAtlasExplorer/CMake
)
