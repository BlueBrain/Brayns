# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSENGINE_SOURCES
  Camera.cpp
  Engine.cpp
  FrameBuffer.cpp
  LightManager.cpp
  Material.cpp
  Model.cpp
  Renderer.cpp
  Scene.cpp
  SharedDataVolume.cpp
  Volume.cpp
)

set(BRAYNSENGINE_PUBLIC_HEADERS
  BrickedVolume.h
  Camera.h
  Engine.h
  FrameBuffer.h
  LightManager.h
  Material.h
  Model.h
  Renderer.h
  Scene.h
  SharedDataVolume.h
  Volume.h
)

set(BRAYNSENGINE_INCLUDE_NAME brayns/engine)
set(BRAYNSENGINE_LINK_LIBRARIES
  PUBLIC braynsCommon
  PRIVATE braynsParameters ${FREEIMAGE_LIBRARIES} ${Boost_FILESYSTEM_LIBRARY}
                           ${Boost_SYSTEM_LIBRARY}
)

common_library(braynsEngine)
