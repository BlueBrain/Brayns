# Copyright (c) 2011-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of BRayns

add_subdirectory(parameters)
add_subdirectory(common)
add_subdirectory(io)

set(BRAYNS_PUBLIC_HEADERS Brayns.h)

set(BRAYNS_SOURCES Brayns.cpp)

set(BRAYNS_LINK_LIBRARIES
    PUBLIC Servus braynsParameters braynsCommon
           braynsIO braynsPlugins braynsOSPRayPlugin ${OSPRAY_LIBRARIES})

common_library(brayns)
