# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

add_subdirectory(parameters)
add_subdirectory(common)
add_subdirectory(io)

set(BRAYNS_PUBLIC_HEADERS Brayns.h)

set(BRAYNS_SOURCES Brayns.cpp)

set(BRAYNS_LINK_LIBRARIES
    PUBLIC braynsParameters braynsCommon braynsIO braynsPlugins)

if(BRAYNS_IMAGEMAGICK_ENABLED)
  list(APPEND BRAYNS_LINK_LIBRARIES PRIVATE ${Magick++_LIBRARIES})
endif()

if(TARGET Lunchbox)
  list(APPEND BRAYNS_LINK_LIBRARIES PRIVATE Lunchbox)
endif()

common_library(brayns)
