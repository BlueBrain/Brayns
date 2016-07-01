# Copyright (c) 2015-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSUI_SOURCES
  Viewport.cpp
  BaseWindow.cpp
  gl/ScreenSpaceProcessor.cpp
  gl/shaders.cpp
  manipulators/AbstractManipulator.cpp
  manipulators/InspectCenterManipulator.cpp
  manipulators/FlyingModeManipulator.cpp
)

set(BRAYNSUI_PUBLIC_HEADERS
  Viewport.h
  BaseWindow.h
  gl/ScreenSpaceProcessor.h
  gl/shaders.h
  gl/types.h
  manipulators/AbstractManipulator.h
  manipulators/InspectCenterManipulator.h
  manipulators/FlyingModeManipulator.h
)

set(BRAYNSUI_LINK_LIBRARIES
    PUBLIC Servus brayns braynsParameters boost_filesystem vmmlib boost_system
    PRIVATE ${GLUT_LIBRARIES} ${GLEW_LIBRARIES}
)

common_library(braynsUI)
