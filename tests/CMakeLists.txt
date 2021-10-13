# Copyright (c) 2016-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Daniel.Nachbaur@epfl.ch
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(TEST_LIBRARIES brayns braynsIO braynsManipulators)

configure_file(paths.h.in ${PROJECT_BINARY_DIR}/tests/paths.h)

list(APPEND EXCLUDE_FROM_TESTS myPlugin.cpp)

if(TARGET pdiff)
  list(APPEND TEST_LIBRARIES pdiff ${FREEIMAGE_LIBRARIES})
else()
  list(APPEND EXCLUDE_FROM_TESTS
    clipPlaneRendering.cpp
    snapshot.cpp
    streamlines.cpp
  )
endif()

if(NOT BRAYNS_OSPRAY_ENABLED)
  list(APPEND EXCLUDE_FROM_TESTS
    brayns.cpp
    clipPlaneRendering.cpp
    shadows.cpp
    streamlines.cpp
    subsampling.cpp
    testImages.cpp
    lights.cpp
  )
else()
  list(APPEND TEST_LIBRARIES braynsOSPRayEngine)
endif()

if(TARGET braynsCircuitViewer)
  list(APPEND TEST_LIBRARIES braynsCircuitViewer)
else()
  list(APPEND EXCLUDE_FROM_TESTS shadows.cpp)
endif()

if(BRAYNS_OSPRAY_ENABLED)
  list(APPEND CMAKE_MODULE_PATH ${OSPRAY_CMAKE_ROOT})
  include(osprayUse)
  include_directories_ispc(${PROJECT_SOURCE_DIR})
  ospray_ispc_compile(${PROJECT_SOURCE_DIR}/engines/ospray/ispc/render/BasicRenderer.ispc)
endif()

list(APPEND LCOV_EXCLUDE '${PROJECT_SOURCE_DIR}/brayns/common/utils/base64/*')
include(CommonCTest)
