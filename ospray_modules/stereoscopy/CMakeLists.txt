# Copyright (c) 2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(ospray_module_stereoscopy VERSION 0.1.0)
set(ospray_module_stereoscopy_VERSION_ABI 1)

include(Common)

set(OSPRAY_MODULE_STEREOSCOPY_SOURCES
    stereoscopy.cpp
    ispc/stereo/StereoCamera.cpp)

set(OSPRAY_MODULE_STEREOSCOPY_ISPC_SOURCES
    ispc/stereo/StereoCamera.ispc)

# Compile ispc code
list(APPEND CMAKE_MODULE_PATH ${OSPRAY_CMAKE_ROOT})
list(APPEND ALL_ISPC_INCLUDES ${OSPRAY_INCLUDE_DIRS})
include_directories_ispc(${ALL_ISPC_INCLUDES})
ospray_ispc_compile(${OSPRAY_MODULE_STEREOSCOPY_ISPC_SOURCES})
list(APPEND OSPRAY_MODULE_STEREOSCOPY_SOURCES ${ISPC_OBJECTS})

# Compile C++ code
set(OSPRAY_MODULE_STEREOSCOPY_LINK_LIBRARIES PRIVATE ${OSPRAY_LIBRARIES})
set(OSPRAY_MODULE_STEREOSCOPY_OMIT_VERSION_HEADERS ON)
common_library(ospray_module_stereoscopy)

set(DOXYGEN_MAINPAGE_MD README.md)
set(DOXYGEN_EXTRA_INPUT "${PROJECT_SOURCE_DIR}/README.md")
include(DoxygenRule)
