# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSPLUGINAPI_SOURCES
  ExtensionPluginFactory.cpp
)

set(BRAYNSPLUGINAPI_PUBLIC_HEADERS
  ExtensionPlugin.h
  PluginAPI.h
)

set(BRAYNSPLUGINAPI_LINK_LIBRARIES
  PUBLIC braynsCommon
)

set(BRAYNSPLUGINAPI_INCLUDE_NAME brayns/pluginapi)
common_library(braynsPluginAPI)
