# Copyright (c) 2015-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSVIEWER_SOURCES BraynsViewer.cpp main.cpp)

set(BRAYNSVIEWER_HEADERS
  BraynsViewer.h
)

set(BRAYNSVIEWER_LINK_LIBRARIES
  PUBLIC brayns braynsCommon braynsIO braynsParameters braynsUI ${OPENGL_gl_LIBRARY}
)

common_application(braynsViewer)
