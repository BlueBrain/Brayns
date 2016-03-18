# Copyright (c) HBP 2014-2015 cyrille.favreau@epfl.ch
# All rights reserved. Do not distribute without further notice.

set(BRAYNSVIEWER_SOURCES BraynsViewer.cpp main.cpp)

set(BRAYNSVIEWER_HEADERS
  BraynsViewer.h
)

set(BRAYNSVIEWER_LINK_LIBRARIES
  brayns
  braynsCommon
  braynsPlugins
  braynsOSPRayPlugin
)

common_application(braynsViewer)
