# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

add_subdirectory(parameters)
add_subdirectory(common)
add_subdirectory(io)
add_subdirectory(tasks)
add_subdirectory(pluginapi)

set(BRAYNS_PUBLIC_HEADERS Brayns.h)
set(BRAYNS_HEADERS EngineFactory.h)
set(BRAYNS_SOURCES
  Brayns.cpp
  EngineFactory.cpp
)

set(BRAYNS_LINK_LIBRARIES
  PUBLIC braynsParameters braynsCommon
  PRIVATE braynsIO braynsPluginAPI braynsTasks ${OSPRAY_COMMON_LIBRARY})

if(TARGET braynsOSPRayEnginePlugin)
  list(APPEND BRAYNS_LINK_LIBRARIES PRIVATE braynsOSPRayEnginePlugin)
endif()

if(TARGET DeflectPlugin)
  list(APPEND BRAYNS_LINK_LIBRARIES PRIVATE DeflectPlugin)
endif()

if(TARGET RocketsPlugin)
  list(APPEND BRAYNS_LINK_LIBRARIES PRIVATE RocketsPlugin)
endif()

set(BRAYNS_OMIT_EXPORT ON)
common_library(brayns)
