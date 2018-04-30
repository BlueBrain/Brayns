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

set(BRAYNS_SOURCES Brayns.cpp)

set(BRAYNS_LINK_LIBRARIES
  PUBLIC braynsParameters braynsCommon braynsTasks
  PRIVATE braynsIO braynsPlugins braynsPluginAPI ${OSPRAY_COMMON_LIBRARY})

if(TARGET Lunchbox)
  list(APPEND BRAYNS_LINK_LIBRARIES PRIVATE Lunchbox)
endif()

common_library(brayns)
