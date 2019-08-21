# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(DeflectPlugin VERSION 1.0.0)
set(DeflectPlugin_VERSION_ABI 1)

include(Common)

common_find_package(Deflect REQUIRED)
common_find_package(ospray 1.8 SYSTEM)
common_find_package_post()

set(BRAYNSDEFLECT_SOURCES
  DeflectParameters.cpp
  DeflectPlugin.cpp
)
set(BRAYNSDEFLECT_HEADERS
  DeflectParameters.h
  DeflectPlugin.h
  utils.h
)

set(BRAYNSDEFLECT_LINK_LIBRARIES PRIVATE braynsCommon braynsEngine
  braynsManipulators braynsParameters Deflect)

if(OSPRAY_FOUND)
  list(APPEND BRAYNSDEFLECT_SOURCES DeflectPixelOp.cpp)
  list(APPEND BRAYNSDEFLECT_HEADERS DeflectPixelOp.h)
  list(APPEND BRAYNSDEFLECT_LINK_LIBRARIES PUBLIC ospray::ospray_common ospray::ospray
    PRIVATE braynsOSPRayEngine)
endif()

if(libuv_FOUND)
  list(APPEND BRAYNSDEFLECT_LINK_LIBRARIES PRIVATE ${libuv_LIBRARIES})
endif()

set(BRAYNSDEFLECT_OMIT_LIBRARY_HEADER ON)
set(BRAYNSDEFLECT_OMIT_VERSION_HEADERS ON)
set(BRAYNSDEFLECT_OMIT_EXPORT ON)
set(BRAYNSDEFLECT_INCLUDE_NAME deflect)
common_library(braynsDeflect)

if(TARGET Brayns-all)
  add_dependencies(Brayns-all braynsDeflect)
endif()
