# Copyright (c) 2020, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

project(braynsPBRTEngine VERSION 1.0.0)

set(BRAYNSPBRTENGINE_SOURCES
  PBRTCamera.cpp
  PBRTContants.cpp
  PBRTEngine.cpp
  PBRTFrameBuffer.cpp
  PBRTMaterial.cpp
  PBRTModel.cpp
  PBRTRenderer.cpp
  PBRTScene.cpp
  util/PBRTDebugIntegrator.cpp
  util/TransformPool.cpp
  util/Util.cpp
)

set(BRAYNSPBRTENGINE_HEADERS
  PBRTCamera.h
  PBRTConstants.h
  PBRTEngine.h
  PBRTFrameBuffer.h
  PBRTMaterial.h
  PBRTModel.h
  PBRTRenderer.h
  PBRTScene.h
  util/PBRTDebugIntegrator.h
  util/PBRTSDFGeometryShape.h
  util/PBRTTextureImpl.h
  util/TransformPool.h
  util/Util.h
)

set(BRAYNSPBRTENGINE_INCLUDE_DIR ${PROJECT_SOURCE_DIR})

add_library(braynsPBRTEngine SHARED ${BRAYNSPBRTENGINE_SOURCES} ${BRAYNSPBRTENGINE_HEADERS})
target_compile_options(braynsPBRTEngine PRIVATE -Wall -Wextra -pedantic -O3 -fno-strict-aliasing)
target_include_directories(braynsPBRTEngine PRIVATE ${BRAYNSPBRTENGINE_INCLUDE_DIR})
target_link_libraries(braynsPBRTEngine PRIVATE braynsEngine braynsParameters braynsCommon pbrt)

install(TARGETS braynsPBRTEngine
    EXPORT braynsPBRTEngine-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(EXPORT braynsPBRTEngine-targets
    DESTINATION share/engines/pbrt/CMake
)
