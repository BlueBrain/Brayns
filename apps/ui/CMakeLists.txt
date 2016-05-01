# Copyright (c) 2011-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of BRayns

set(BRAYNSUI_SOURCES
  Viewport.cpp
  BaseWindow.cpp
  manipulators/AbstractManipulator.cpp
  manipulators/InspectCenterManipulator.cpp
  manipulators/FlyingModeManipulator.cpp
)

set(BRAYNSUI_PUBLIC_HEADERS
  Viewport.h
  BaseWindow.h
  manipulators/AbstractManipulator.h
  manipulators/InspectCenterManipulator.h
  manipulators/FlyingModeManipulator.h
)

set(BRAYNSUI_LINK_LIBRARIES
    PUBLIC Servus brayns braynsParameters boost_filesystem vmmlib boost_system
    PRIVATE ${GLUT_LIBRARIES}
)

common_library(braynsUI)
