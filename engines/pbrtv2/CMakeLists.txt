# Copyright (c) 2020, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

project(braynsPBRTV2Engine VERSION 1.0.0)

set(BRAYNSPBRTV2ENGINE_SOURCES
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

set(BRAYNSPBRTV2ENGINE_HEADERS
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

set(BRAYNSPBRTV2ENGINE_INCLUDE_DIR ${PROJECT_SOURCE_DIR})

add_library(braynsPBRTV2Engine SHARED ${BRAYNSPBRTV2ENGINE_SOURCES} ${BRAYNSPBRTV2ENGINE_HEADERS})
target_compile_options(braynsPBRTV2Engine PRIVATE -Wall -Wextra -pedantic -O3 -fno-strict-aliasing)
target_compile_definitions(braynsPBRTV2Engine PUBLIC PBRT_PROBES_NONE)
target_include_directories(braynsPBRTV2Engine PRIVATE ${BRAYNSPBRTV2ENGINE_INCLUDE_DIR})
target_link_libraries(braynsPBRTV2Engine PRIVATE braynsEngine braynsParameters braynsCommon pbrtlibv2)

install(TARGETS braynsPBRTV2Engine
    EXPORT braynsPBRTV2Engine-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(EXPORT braynsPBRTV2Engine-targets
    DESTINATION share/engines/pbrtv2/CMake
)
