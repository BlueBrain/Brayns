# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

add_subdirectory(parameters)
add_subdirectory(common)
add_subdirectory(io)

set(BRAYNS_PUBLIC_HEADERS Brayns.h PluginAPI.h)

set(BRAYNS_SOURCES Brayns.cpp)

set(BRAYNS_LINK_LIBRARIES
  PUBLIC braynsParameters braynsCommon
         ${EMBREE_LIBRARY} # embree has to be exposed PUBLIC, don't know why
  PRIVATE braynsIO braynsPlugins)

if(TARGET Lunchbox)
  list(APPEND BRAYNS_LINK_LIBRARIES PRIVATE Lunchbox)
endif()

common_library(brayns)
