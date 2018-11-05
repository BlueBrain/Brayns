# Copyright (c) 2016-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Daniel.Nachbaur@epfl.ch
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(TEST_LIBRARIES ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                   ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY}
                   brayns braynsIO)

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
    addModel.cpp
    addModelFromBlob.cpp
    clipPlaneRendering.cpp
    brayns.cpp
    model.cpp
    plugin.cpp
    renderer.cpp
    snapshot.cpp
    streamlines.cpp
    testImages.cpp
    webAPI.cpp
    shadows.cpp
    transferFunction.cpp
  )
endif()

if(NOT BRAYNS_ASSIMP_ENABLED)
  list(APPEND EXCLUDE_FROM_TESTS
    addModel.cpp
    addModelFromBlob.cpp
  )
endif()

if(BRAYNS_NETWORKING_ENABLED AND BRAYNS_OSPRAY_ENABLED)
  list(APPEND CMAKE_MODULE_PATH ${OSPRAY_CMAKE_ROOT})
  include(ispc)
  include_directories_ispc(${PROJECT_SOURCE_DIR} ${OSPRAY_INCLUDE_DIRS})
  ospray_ispc_compile(${PROJECT_SOURCE_DIR}/engines/ospray/ispc/render/BasicRenderer.ispc)
  add_library(myPlugin SHARED
    myPlugin.cpp
    ${PROJECT_SOURCE_DIR}/plugins/Rockets/staticjson/staticjson.cpp
    ${ISPC_OBJECTS}
  )
  target_link_libraries(myPlugin
    brayns
    braynsOSPRayEngine
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    ${OSPRAY_LIBRARIES}
  )
  target_include_directories(myPlugin SYSTEM PRIVATE
    ${PROJECT_SOURCE_DIR}/plugins/Rockets
    ${PROJECT_SOURCE_DIR}/plugins/Rockets/rapidjson/include)
  list(APPEND TEST_LIBRARIES Rockets braynsRockets myPlugin)
else()
  list(APPEND EXCLUDE_FROM_TESTS
    addModel.cpp
    addModelFromBlob.cpp
    clipPlanes.cpp
    model.cpp
    plugin.cpp
    renderer.cpp
    snapshot.cpp
    throttle.cpp
    transferFunction.cpp
    webAPI.cpp
  )
endif()

option(BRAYNS_OPTIX_TESTS_ENABLED "Activate OptiX tests" ON)
if(NOT BRAYNS_OPTIX_ENABLED OR NOT BRAYNS_OPTIX_TESTS_ENABLED)
    list(APPEND EXCLUDE_FROM_TESTS demoOptiX.cpp)
endif()

include_directories(
  ${PROJECT_SOURCE_DIR}/plugins/Rockets
  ${PROJECT_SOURCE_DIR}/plugins/Rockets/rapidjson/include
)

list(APPEND LCOV_EXCLUDE '${PROJECT_SOURCE_DIR}/brayns/common/utils/base64/*'
                         '${PROJECT_SOURCE_DIR}/plugins/Rockets/rapidjson/*'
                         '${PROJECT_SOURCE_DIR}/plugins/Rockets/staticjson/*')
include(CommonCTest)
