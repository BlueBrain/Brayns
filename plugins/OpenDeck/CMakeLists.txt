# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(BraynsOpenDeck VERSION 1.0.0)
set(BraynsOpenDeck_VERSION_ABI 1)

include(Common)

common_find_package(ospray 1.8 SYSTEM)
common_find_package_post()


set(BRAYNSOPENDECK_HEADERS OpenDeckPlugin.h OpenDeckParameters.h)
set(BRAYNSOPENDECK_SOURCES OpenDeckPlugin.cpp OpenDeckParameters.cpp)
set(BRAYNSOPENDECK_LINK_LIBRARIES PRIVATE braynsCommon braynsEngine braynsPluginAPI)

if(OSPRAY_FOUND)
  list(APPEND BRAYNSOPENDECK_SOURCES
    ispc/cylindric/CylindricCamera.cpp
    ispc/cylindricStereo/CylindricStereoCamera.cpp
    ispc/cylindricStereoTracked/CylindricStereoTrackedCamera.cpp)

  set(BRAYNSOPENDECK_ISPC_SOURCES
    ispc/cylindric/CylindricCamera.ispc
    ispc/cylindricStereo/CylindricStereoCamera.ispc
    ispc/cylindricStereoTracked/CylindricStereoTrackedCamera.ispc)

  list(APPEND BRAYNSOPENDECK_SOURCES ${BRAYNSOPENDECK_ISPC_SOURCES})

  # reuse ispc setup and macros from ospray
  list(APPEND CMAKE_MODULE_PATH ${OSPRAY_CMAKE_ROOT})
  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(OSPRAY_DEBUG_BUILD ON)
  endif()
  include(ispc)

  # Compile ispc code
  include_directories_ispc(${PROJECT_SOURCE_DIR}/../../ ${OSPRAY_INCLUDE_DIRS})
  ospray_ispc_compile(${BRAYNSOPENDECK_ISPC_SOURCES})
  list(APPEND BRAYNSOPENDECK_SOURCES ${ISPC_OBJECTS})

  list(APPEND BRAYNSOPENDECK_LINK_LIBRARIES ospray::ospray_common ospray::ospray ospray::ospray_module_ispc)
endif()

set(BRAYNSOPENDECK_OMIT_LIBRARY_HEADER ON)
set(BRAYNSOPENDECK_OMIT_VERSION_HEADERS ON)
set(BRAYNSOPENDECK_OMIT_EXPORT ON)
set(BRAYNSOPENDECK_INCLUDE_NAME brayns_opendeck)
common_library(braynsOpenDeck)

if (TARGET Brayns-all)
  add_dependencies(Brayns-all braynsOpenDeck)
endif()
