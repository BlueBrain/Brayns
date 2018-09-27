# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(VRPNPlugin VERSION 1.0.0)
set(VRPNPlugin_VERSION_ABI 1)

include(Common)

common_find_package(VRPN REQUIRED)
common_find_package(libuv)
common_find_package_post()

set(VRPNPLUGIN_HEADERS VRPNPlugin.h)
set(VRPNPLUGIN_SOURCES VRPNPlugin.cpp)
set(VRPNPLUGIN_LINK_LIBRARIES PRIVATE braynsPluginAPI ${VRPN_LIBRARIES})

set(VRPNPLUGIN_OMIT_LIBRARY_HEADER ON)
set(VRPNPLUGIN_OMIT_VERSION_HEADERS ON)
set(VRPNPLUGIN_OMIT_EXPORT ON)
set(VRPNPLUGIN_INCLUDE_NAME vrpnplugin)
common_library(VRPNPlugin)
