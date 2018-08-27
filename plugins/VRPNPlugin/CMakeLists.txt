# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(VRPNPLUGIN_HEADERS VRPNPlugin.h)
set(VRPNPLUGIN_SOURCES VRPNPlugin.cpp)
set(VRPNPLUGIN_LINK_LIBRARIES PRIVATE braynsPluginAPI vmmlib ${VRPN_LIBRARIES})

set(VRPNPLUGIN_OMIT_LIBRARY_HEADER ON)
set(VRPNPLUGIN_OMIT_VERSION_HEADERS ON)
set(VRPNPLUGIN_OMIT_EXPORT ON)
set(VRPNPLUGIN_INCLUDE_NAME vrpnplugin)
common_library(VRPNPlugin)
