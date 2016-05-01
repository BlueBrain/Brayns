# Copyright (c) 2011-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of BRayns

set(BRAYNSVIEWER_SOURCES BraynsViewer.cpp main.cpp)

set(BRAYNSVIEWER_HEADERS
  BraynsViewer.h
)

set(BRAYNSVIEWER_LINK_LIBRARIES
  PUBLIC brayns braynsCommon braynsIO braynsParameters braynsUI
)

common_application(braynsViewer)
