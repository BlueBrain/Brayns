# Copyright (c) 2015-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSCOMMONENGINEPLUGIN_SOURCES
  Engine.cpp
)

set(BRAYNSCOMMONENGINEPLUGIN_HEADERS
  Engine.h
)

set(BRAYNSCOMMONENGINEPLUGIN_LINK_LIBRARIES
    PUBLIC vmmlib
)

common_library(braynsCommonEnginePlugin)
