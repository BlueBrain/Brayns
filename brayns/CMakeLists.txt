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
    PUBLIC Servus braynsParameters braynsCommon braynsIO braynsPlugins)

common_library(brayns)
