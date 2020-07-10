# Copyright (c) 2019, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(braynsCircuitRenderer VERSION 0.1.0)
set(braynsCircuitRenderer_VERSION_ABI 1)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
                              ${CMAKE_SOURCE_DIR}/CMake/common)
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/CMake/common/Common.cmake)
  message(FATAL_ERROR "CMake/common missing, run: git submodule update --init --recursive")
endif()

include(Common)
common_find_package(ospray 1.8 SYSTEM)
common_find_package_post()

list(APPEND CMAKE_MODULE_PATH ${OSPRAY_CMAKE_ROOT})
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(OSPRAY_DEBUG_BUILD ON)
endif()
include(ispc)

set(BRAYNSCIRCUITRENDERER_SOURCES
  CircuitRendererPlugin.cpp
  ispc/TestRenderer.cpp
  ispc/TestMaterial.cpp
)

set(BRAYNSCIRCUITRENDERER_HEADERS
  CircuitRendererPlugin.h
  Log.h
)

set(BRAYNSCIRCUITRENDERER_ISPC_SOURCES
    ispc/TestRenderer.ispc
    ispc/TestMaterial.ispc
)

# Compile ispc code
list(APPEND ALL_ISPC_INCLUDES ${OSPRAY_INCLUDE_DIRS})
include_directories_ispc(${ALL_ISPC_INCLUDES})
ospray_ispc_compile(${BRAYNSCIRCUITRENDERER_ISPC_SOURCES})
list(APPEND BRAYNSCIRCUITRENDERER_SOURCES ${ISPC_OBJECTS})

# Compile C++ code
include_directories(
    SYSTEM
    ${CMAKE_CURRENT_SOURCE_DIR}/.
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

set(BRAYNSCIRCUITRENDERER_LINK_LIBRARIES
    PUBLIC ${OSPRAY_LIBRARIES}
    PRIVATE braynsCommon braynsParameters braynsEngine braynsPluginAPI braynsOSPRayEngine glm)

set(BRAYNSCIRCUITRENDERER_OMIT_LIBRARY_HEADER ON)
set(BRAYNSCIRCUITRENDERER_OMIT_VERSION_HEADERS ON)
set(BRAYNSCIRCUITRENDERER_OMIT_EXPORT ON)

common_library(braynsCircuitRenderer)
