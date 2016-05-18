# Copyright (c) 2011-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of BRayns

set(BRAYNSPLUGINS_LINK_LIBRARIES
  PUBLIC braynsCommon vmmlib)

set(BRAYNSPLUGINS_SOURCES
  extensions/plugins/ExtensionPlugin.cpp
)

set(BRAYNSPLUGINS_PUBLIC_HEADERS
  extensions/plugins/ExtensionPlugin.h
)

if(DEFLECT_FOUND OR (TARGET ZeroBuf AND TARGET ZeroEQ))
  list(APPEND BRAYNSPLUGINS_SOURCES extensions/ExtensionPluginFactory.cpp)
  list(APPEND BRAYNSPLUGINS_PUBLIC_HEADERS extensions/ExtensionPluginFactory.h)
endif()

if(DEFLECT_FOUND)
  list(APPEND BRAYNSPLUGINS_SOURCES extensions/plugins/DeflectPlugin.cpp)
  list(APPEND BRAYNSPLUGINS_PUBLIC_HEADERS extensions/plugins/DeflectPlugin.h)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES Deflect)
endif()

if(TARGET Lexis AND TARGET ZeroEQ)
  list(APPEND BRAYNSPLUGINS_SOURCES extensions/plugins/ZeroBufPlugin.cpp)
  list(APPEND BRAYNSPLUGINS_PUBLIC_HEADERS extensions/plugins/ZeroBufPlugin.h)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES
    PUBLIC Lexis ZeroEQ BraynsZeroBufRender ${LibJpegTurbo_LIBRARIES})
  add_subdirectory(extensions/vocabulary)
endif()

if(MAGICK++_FOUND)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES Magick++)
endif()

add_subdirectory(renderers/ospray)

common_library(braynsPlugins)
