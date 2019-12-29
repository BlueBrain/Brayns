# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSPLUGINAPI_PUBLIC_HEADERS
  ExtensionPlugin.h
  Plugin.h
)

set(BRAYNSPLUGINAPI_INCLUDE_NAME brayns/pluginapi)
common_library(braynsPluginAPI)
