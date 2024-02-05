# Copyright 2015-2024 Blue Brain Project/EPFL
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

list(APPEND CMAKE_MODULE_PATH ${OSPRAY_CMAKE_ROOT})
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(OSPRAY_DEBUG_BUILD ON)
endif()
include(ispc)

set(BRAYNSOPENDECK_HEADERS
    OpenDeckPlugin.h
    OpenDeckParameters.h
)

set(BRAYNSOPENDECK_SOURCES
    OpenDeckPlugin.cpp
    OpenDeckParameters.cpp
    ispc/cylindric/CylindricCamera.cpp
    ispc/cylindricStereo/CylindricStereoCamera.cpp
    ispc/cylindricStereoTracked/CylindricStereoTrackedCamera.cpp
)

set(BRAYNSOPENDECK_ISPC_SOURCES
  ispc/cylindric/CylindricCamera.ispc
  ispc/cylindricStereo/CylindricStereoCamera.ispc
  ispc/cylindricStereoTracked/CylindricStereoTrackedCamera.ispc
)
list(APPEND BRAYNSOPENDECK_SOURCES ${BRAYNSOPENDECK_ISPC_SOURCES})

include_directories_ispc(${PROJECT_SOURCE_DIR} ${OSPRAY_INCLUDE_DIRS})
ospray_ispc_compile(${BRAYNSOPENDECK_ISPC_SOURCES})
list(APPEND BRAYNSOPENDECK_SOURCES ${ISPC_OBJECTS})

add_library(braynsOpenDeck SHARED ${BRAYNSOPENDECK_HEADERS} ${BRAYNSOPENDECK_SOURCES})

target_link_libraries(braynsOpenDeck PRIVATE
    braynsCommon braynsEngine braynsPluginAPI braynsNetwork
    ospray::ospray_common ospray::ospray ospray::ospray_module_ispc
)

set_target_properties(braynsOpenDeck
    PROPERTIES
        VERSION ${BRAYNS_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsOpenDeck
    EXPORT braynsOpenDeck-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(EXPORT braynsOpenDeck-targets
    DESTINATION share/braynsOpenDeck/CMake
)


