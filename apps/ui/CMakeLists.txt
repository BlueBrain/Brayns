# Copyright (c) 2015-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSUI_SOURCES
  BaseWindow.cpp
  gl/ScreenSpaceProcessor.cpp
  gl/shaders.cpp
)

set(BRAYNSUI_PUBLIC_HEADERS
  BaseWindow.h
  gl/ScreenSpaceProcessor.h
  gl/shaders.h
)

include(StringifyShaders)
set(SSP_SHADERS gl/shaders/fragQuad.glsl
                gl/shaders/vertQuad.glsl)

stringify_shaders(${SSP_SHADERS})
list(APPEND BRAYNSUI_SOURCES ${SHADER_SOURCES})

set(BRAYNSUI_LINK_LIBRARIES
    PUBLIC Servus brayns braynsParameters boost_filesystem vmmlib boost_system
    PRIVATE ${GLUT_LIBRARIES} ${GLEW_LIBRARIES}
)

common_library(braynsUI)
