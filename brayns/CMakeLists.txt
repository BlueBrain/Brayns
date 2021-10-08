# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

add_subdirectory(parameters)
add_subdirectory(common)
add_subdirectory(engine)
add_subdirectory(manipulators)
add_subdirectory(io)
add_subdirectory(tasks)
add_subdirectory(pluginapi)
add_subdirectory(network)

set(BRAYNS_PUBLIC_HEADERS Brayns.h)
set(BRAYNS_HEADERS EngineFactory.h)
set(BRAYNS_SOURCES
  Brayns.cpp
  EngineFactory.cpp
  PluginManager.cpp
)

set(BRAYNS_LINK_LIBRARIES
  PUBLIC braynsParameters braynsCommon braynsEngine braynsNetwork
  PRIVATE braynsIO braynsPluginAPI braynsManipulators braynsTasks)

set(BRAYNS_OMIT_EXPORT ON)
common_library(brayns)
