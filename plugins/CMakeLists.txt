# Copyright (c) 2011-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of BRayns

set(BRAYNSPLUGINS_LINK_LIBRARIES PUBLIC braynsCommon vmmlib)

set(BRAYNSPLUGINS_SOURCES
  loaders/MorphologyLoader.cpp
  loaders/ProteinLoader.cpp
  extensions/ExtensionPluginFactory.cpp
  extensions/plugins/ExtensionPlugin.cpp
)

set(BRAYNSPLUGINS_HEADERS
  loaders/MorphologyLoader.h
  loaders/ProteinLoader.h
  extensions/ExtensionPluginFactory.h
  extensions/plugins/ExtensionPlugin.h
)

if(TARGET Brion)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES Brion Brain)
endif()

if(ASSIMP_FOUND)
  list(APPEND BRAYNSPLUGINS_SOURCES loaders/MeshLoader.cpp)
  list(APPEND BRAYNSPLUGINS_HEADERS loaders/MeshLoader.h)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES assimp)
endif()

if(DEFLECT_FOUND)
  list(APPEND BRAYNSPLUGINS_SOURCES extensions/plugins/DeflectPlugin.cpp)
  list(APPEND BRAYNSPLUGINS_HEADERS extensions/plugins/DeflectPlugin.h)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES Deflect)
endif()

if(TARGET ZeroBuf AND TARGET zeq)
  list(APPEND BRAYNSPLUGINS_SOURCES extensions/plugins/ZeroBufPlugin.cpp)
  list(APPEND BRAYNSPLUGINS_HEADERS extensions/plugins/ZeroBufPlugin.h)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES
    PUBLIC zeq zeqHBP ZeroBuf ZeroBufRender BraynsZeroBufRender ${LibJpegTurbo_LIBRARIES})
  add_subdirectory(extensions/vocabulary)
endif()

if(MAGICK++_FOUND)
  list(APPEND BRAYNSPLUGINS_SOURCES loaders/TextureLoader.cpp)
  list(APPEND BRAYNSPLUGINS_HEADERS loaders/TextureLoader.h)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES Magick++)
endif()

add_subdirectory(renderers/ospray)

common_library(braynsPlugins)
