# Copyright (c) 2015-2017, EPFL/Blue Brain Project
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
    PUBLIC brayns braynsParameters vmmlib
      ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY}
    PRIVATE ${GLUT_LIBRARIES} ${GLEW_LIBRARIES}
)

common_library(braynsUI)
