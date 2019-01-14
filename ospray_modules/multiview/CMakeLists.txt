# Copyright (c) 2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(ospray_module_multiview VERSION 0.1.0)
set(ospray_module_multiview_VERSION_ABI 1)

include(Common)

set(OSPRAY_MODULE_MULTIVIEW_SOURCES
    multiview.cpp
    ispc/multiview/MultiviewCamera.cpp)

set(OSPRAY_MODULE_MULTIVIEW_ISPC_SOURCES
    ispc/multiview/MultiviewCamera.ispc
    ispc/multiview/utils.ispc)

# Compile ispc code
list(APPEND CMAKE_MODULE_PATH ${OSPRAY_CMAKE_ROOT})
list(APPEND ALL_ISPC_INCLUDES ${OSPRAY_INCLUDE_DIRS})
include_directories_ispc(${ALL_ISPC_INCLUDES})
ospray_ispc_compile(${OSPRAY_MODULE_MULTIVIEW_ISPC_SOURCES})
list(APPEND OSPRAY_MODULE_MULTIVIEW_SOURCES ${ISPC_OBJECTS})

# Compile C++ code
set(OSPRAY_MODULE_MULTIVIEW_LINK_LIBRARIES PRIVATE ${OSPRAY_LIBRARIES})
set(OSPRAY_MODULE_MULTIVIEW_OMIT_VERSION_HEADERS ON)
common_library(ospray_module_multiview)

set(DOXYGEN_MAINPAGE_MD README.md)
set(DOXYGEN_EXTRA_INPUT "${PROJECT_SOURCE_DIR}/README.md")
set(COMMON_PROJECT_DOMAIN ch.epfl.bluebrain)
include(DoxygenRule)
