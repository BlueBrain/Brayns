# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(DEFLECTPLUGIN_SOURCES DeflectPlugin.cpp)
set(DEFLECTPLUGIN_HEADERS DeflectPlugin.h)
set(DEFLECTPLUGIN_LINK_LIBRARIES PRIVATE Deflect)

if(libuv_FOUND)
  list(APPEND DEFLECTPLUGIN_LINK_LIBRARIES
    PRIVATE braynsCommon braynsParameters ${libuv_LIBRARIES})
endif()

set(DEFLECTPLUGIN_OMIT_LIBRARY_HEADER ON)
set(DEFLECTPLUGIN_OMIT_VERSION_HEADERS ON)
set(DEFLECTPLUGIN_OMIT_EXPORT ON)
set(DEFLECTPLUGIN_INCLUDE_NAME deflectplugin)
common_library(DeflectPlugin)
