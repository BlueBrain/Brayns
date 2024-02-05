# Copyright 2015-2024 Blue Brain Project/EPFL
# All rights reserved. Do not distribute without permission.
# Responsible Authors: Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
#                      Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

list(APPEND CMAKE_MODULE_PATH ${OSPRAY_CMAKE_ROOT})
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(OSPRAY_DEBUG_BUILD ON)
endif()
include(ispc)

set(BRAYNSMULTIVIEW_SOURCES
    MultiviewPlugin.cpp
    ispc/multiview/MultiviewCamera.cpp
)

set(BRAYNSMULTIVIEW_ISPC_SOURCES
    ispc/multiview/MultiviewCamera.ispc
)

list(APPEND BRAYNSMULTIVIEW_SOURCES ${BRAYNSMULTIVIEW_ISPC_SOURCES})

set(BRAYNSMULTIVIEW_HEADERS
    MultiviewPlugin.h
)

include_directories_ispc(${PROJECT_SOURCE_DIR} ${OSPRAY_INCLUDE_DIRS})
ospray_ispc_compile(${BRAYNSMULTIVIEW_ISPC_SOURCES})
list(APPEND BRAYNSMULTIVIEW_SOURCES ${ISPC_OBJECTS})

add_library(braynsMultiview MultiviewPlugin.cpp MultiviewPlugin.h)

target_link_libraries(braynsMultiview PRIVATE
    braynsCommon braynsEngine braynsPluginAPI braynsParameters braynsOSPRayEngine braynsNetwork
    ospray::ospray_common ospray::ospray ospray::ospray_module_ispc)

set_target_properties(braynsMultiview
    PROPERTIES
        VERSION ${BRAYNS_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsMultiview
    EXPORT braynsMultiview-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(EXPORT braynsMultiview-targets
    DESTINATION share/braynsMultiview/CMake
)

