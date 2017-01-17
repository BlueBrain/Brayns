# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSPLUGINS_LINK_LIBRARIES
  PUBLIC braynsCommon vmmlib)

set(BRAYNSPLUGINS_SOURCES
  extensions/plugins/ExtensionPlugin.cpp
  engines/EngineFactory.cpp
)

set(BRAYNSPLUGINS_PUBLIC_HEADERS
  extensions/plugins/ExtensionPlugin.h
  engines/EngineFactory.h
)

if(TARGET Deflect OR (TARGET Lexis AND TARGET ZeroBuf AND TARGET ZeroEQ))
  list(APPEND BRAYNSPLUGINS_SOURCES extensions/ExtensionPluginFactory.cpp)
  list(APPEND BRAYNSPLUGINS_PUBLIC_HEADERS extensions/ExtensionPluginFactory.h)
endif()

if(TARGET Deflect AND TARGET Lexis)
  list(APPEND BRAYNSPLUGINS_SOURCES extensions/plugins/DeflectPlugin.cpp)
  list(APPEND BRAYNSPLUGINS_PUBLIC_HEADERS extensions/plugins/DeflectPlugin.h)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES Deflect Lexis)
endif()

if(TARGET Lexis AND TARGET ZeroBuf AND TARGET ZeroEQ AND TARGET ZeroEQHTTP)
  list(APPEND BRAYNSPLUGINS_SOURCES extensions/plugins/ZeroEQPlugin.cpp)
  list(APPEND BRAYNSPLUGINS_PUBLIC_HEADERS extensions/plugins/ZeroEQPlugin.h)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES
    PUBLIC Lexis ZeroEQ ZeroEQHTTP BraynsZeroBufRender ${LibJpegTurbo_LIBRARIES})
endif()

if(OSPRAY_FOUND)
  add_subdirectory(engines/ospray)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES PUBLIC braynsOSPRayEnginePlugin)
endif()

if(OPTIX_FOUND AND CUDA_FOUND)
  add_subdirectory(engines/optix)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES PUBLIC braynsOptiXEnginePlugin)
endif()

if(TARGET LivreLib)
  add_subdirectory(engines/livre)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES PUBLIC braynsLivreEnginePlugin)
endif()

common_library(braynsPlugins)
