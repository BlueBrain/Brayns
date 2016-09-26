# Copyright (c) 2015-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSPLUGINS_LINK_LIBRARIES
  PUBLIC braynsCommon vmmlib)

set(BRAYNSPLUGINS_SOURCES
  extensions/plugins/ExtensionPlugin.cpp
  engines/EngineFactory.cpp
  engines/Engine.cpp
)

set(BRAYNSPLUGINS_PUBLIC_HEADERS
  extensions/plugins/ExtensionPlugin.h
  engines/EngineFactory.h
  engines/Engine.h
)

if(DEFLECT_FOUND OR (TARGET Lexis AND TARGET ZeroBuf AND TARGET ZeroEQ))
  list(APPEND BRAYNSPLUGINS_SOURCES extensions/ExtensionPluginFactory.cpp)
  list(APPEND BRAYNSPLUGINS_PUBLIC_HEADERS extensions/ExtensionPluginFactory.h)
endif()

if(DEFLECT_FOUND)
  list(APPEND BRAYNSPLUGINS_SOURCES extensions/plugins/DeflectPlugin.cpp)
  list(APPEND BRAYNSPLUGINS_PUBLIC_HEADERS extensions/plugins/DeflectPlugin.h)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES Deflect)
endif()

if(TARGET Lexis AND TARGET ZeroBuf AND TARGET ZeroEQ)
  list(APPEND BRAYNSPLUGINS_SOURCES extensions/plugins/ZeroEQPlugin.cpp)
  list(APPEND BRAYNSPLUGINS_PUBLIC_HEADERS extensions/plugins/ZeroEQPlugin.h)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES
    PUBLIC Lexis ZeroEQ BraynsZeroBufRender ${LibJpegTurbo_LIBRARIES})
endif()

if(OSPRAY_FOUND)
  add_subdirectory(engines/ospray)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES PUBLIC braynsOSPRayPlugin)
endif()

if(OPTIX_FOUND AND CUDA_FOUND)
  add_subdirectory(engines/optix)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES PUBLIC braynsOptiXPlugin)
endif()

common_library(braynsPlugins)

