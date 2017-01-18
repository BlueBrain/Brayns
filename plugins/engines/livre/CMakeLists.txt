# Copyright (c) 2017, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Daniel.Nachbaur@epfl.ch
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSLIVREENGINEPLUGIN_SOURCES
  LivreCamera.cpp
  LivreEngine.cpp
  LivreFrameBuffer.cpp
  LivreRenderer.cpp
  LivreScene.cpp
)

set(BRAYNSLIVREENGINEPLUGIN_PUBLIC_HEADERS
  LivreCamera.h
  LivreEngine.h
  LivreFrameBuffer.h
  LivreRenderer.h
  LivreScene.h
)

set(BRAYNSLIVREENGINEPLUGIN_LINK_LIBRARIES
    PUBLIC vmmlib braynsCommon braynsIO
           LivreLib LivreEq LivreUVF
)

common_library(braynsLivreEnginePlugin)
