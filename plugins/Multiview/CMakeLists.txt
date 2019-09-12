# Copyright (c) 2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(BraynsMultiview VERSION 0.1.0)
set(BraynsMultiview_VERSION_ABI 1)

include(Common)

common_find_package(ospray 1.8 SYSTEM)
common_find_package_post()

set(BRAYNSMULTIVIEW_HEADERS MultiviewPlugin.h)
set(BRAYNSMULTIVIEW_SOURCES MultiviewPlugin.cpp)
set(BRAYNSMULTIVIEW_LINK_LIBRARIES PRIVATE braynsCommon braynsEngine braynsPluginAPI braynsParameters)

if(OSPRAY_FOUND)
  list(APPEND BRAYNSMULTIVIEW_SOURCES
    ispc/multiview/MultiviewCamera.cpp)

  set(BRAYNSMULTIVIEW_ISPC_SOURCES
    ispc/multiview/MultiviewCamera.ispc)

  list(APPEND BRAYNSMULTIVIEW_SOURCES ${BRAYNSMULTIVIEW_ISPC_SOURCES})

  # reuse ispc setup and macros from ospray
  list(APPEND CMAKE_MODULE_PATH ${OSPRAY_CMAKE_ROOT})
  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(OSPRAY_DEBUG_BUILD ON)
  endif()
  include(ispc)

  # Compile ispc code
  include_directories_ispc(${PROJECT_SOURCE_DIR}/../../ ${OSPRAY_INCLUDE_DIRS})
  ospray_ispc_compile(${BRAYNSMULTIVIEW_ISPC_SOURCES})
  list(APPEND BRAYNSMULTIVIEW_SOURCES ${ISPC_OBJECTS})

  list(APPEND BRAYNSMULTIVIEW_LINK_LIBRARIES braynsOSPRayEngine ospray::ospray_common ospray::ospray ospray::ospray_module_ispc)
endif()

set(BRAYNSMULTIVIEW_OMIT_LIBRARY_HEADER ON)
set(BRAYNSMULTIVIEW_OMIT_VERSION_HEADERS ON)
set(BRAYNSMULTIVIEW_OMIT_EXPORT ON)
set(BRAYNSMULTIVIEW_INCLUDE_NAME brayns_multiview)
common_library(braynsMultiview)

if (TARGET Brayns-all)
  add_dependencies(Brayns-all braynsMultiview)
endif()
