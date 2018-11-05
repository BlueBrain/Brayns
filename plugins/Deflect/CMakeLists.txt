# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSDEFLECT_SOURCES DeflectPlugin.cpp)
set(BRAYNSDEFLECT_HEADERS DeflectPlugin.h)
set(BRAYNSDEFLECT_LINK_LIBRARIES PRIVATE Deflect)

if(libuv_FOUND)
  list(APPEND BRAYNSDEFLECT_LINK_LIBRARIES
    PRIVATE braynsCommon braynsParameters ${libuv_LIBRARIES})
endif()

set(BRAYNSDEFLECT_OMIT_LIBRARY_HEADER ON)
set(BRAYNSDEFLECT_OMIT_VERSION_HEADERS ON)
set(BRAYNSDEFLECT_OMIT_EXPORT ON)
set(BRAYNSDEFLECT_INCLUDE_NAME deflect)
common_library(braynsDeflect)
