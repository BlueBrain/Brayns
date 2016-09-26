# Copyright (c) 2015-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

# Configure the output directories. To allow IMPI to do its magic we
# will put *executables* into the (same) build directory, but tag
# mic-executables with ".mic". *libraries* cannot use the
# ".mic"-suffix trick, so we'll put libraries into separate
# directories (names 'intel64' and 'mic', respectively)
set(BRAYNSOPTIXPLUGIN_SOURCES
  OptiXFrameBuffer.cpp
  OptiXScene.cpp
  OptiXUtils.cpp
  OptiXCamera.cpp
  OptiXRenderer.cpp
  OptiXEngine.cpp
)

set(BRAYNSOPTIXPLUGIN_PUBLIC_HEADERS
  OptiXFrameBuffer.h
  OptiXScene.h
  OptiXUtils.h
  OptiXCamera.h
  OptiXRenderer.h
  OptiXEngine.h
)

set(BRAYNSOPTIXPLUGIN_LINK_LIBRARIES
    PUBLIC vmmlib braynsCommon braynsIO
    PRIVATE ${OptiX_LIBRARIES}
)

add_subdirectory(cuda)

common_library(braynsOptiXPlugin)


