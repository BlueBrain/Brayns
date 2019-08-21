# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(BraynsVRPN VERSION 1.0.0)
set(BraynsVRPN_VERSION_ABI 1)

include(Common)

common_find_package(VRPN REQUIRED)
common_find_package(libuv)
common_find_package_post()

set(BRAYNSVRPN_HEADERS VRPNPlugin.h)
set(BRAYNSVRPN_SOURCES VRPNPlugin.cpp)
set(BRAYNSVRPN_LINK_LIBRARIES PRIVATE braynsCommon braynsPluginAPI braynsEngine
  ${libuv_LIBRARIES} ${VRPN_LIBRARIES})

set(BRAYNSVRPN_OMIT_LIBRARY_HEADER ON)
set(BRAYNSVRPN_OMIT_VERSION_HEADERS ON)
set(BRAYNSVRPN_OMIT_EXPORT ON)
set(BRAYNSVRPN_INCLUDE_NAME brayns_vrpn)
common_library(braynsVRPN)

if (TARGET Brayns-all)
  add_dependencies(Brayns-all braynsVRPN)
endif()
