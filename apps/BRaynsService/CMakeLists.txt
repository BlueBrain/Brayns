# Copyright (c) 2011-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of BRayns

set(BRAYNSSERVICE_SOURCES main.cpp)

set(BRAYNSSERVICE_LINK_LIBRARIES
  brayns
  braynsCommon
  braynsPlugins
  braynsOSPRayPlugin
)

common_application(braynsService)
