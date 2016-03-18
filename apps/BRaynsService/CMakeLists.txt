# Copyright (c) HBP 2014-2015 cyrille.favreau@epfl.ch
# All rights reserved. Do not distribute without further notice.

set(BRAYNSSERVICE_SOURCES main.cpp)

set(BRAYNSSERVICE_LINK_LIBRARIES
  brayns
  braynsCommon
  braynsPlugins
  braynsOSPRayPlugin
)

common_application(braynsService)
