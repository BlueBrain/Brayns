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

if(BRAYNS_DEFLECT_ENABLED OR BRAYNS_NETWORKING_ENABLED)
  list(APPEND BRAYNSPLUGINS_SOURCES extensions/ExtensionPluginFactory.cpp)
  list(APPEND BRAYNSPLUGINS_PUBLIC_HEADERS extensions/ExtensionPluginFactory.h)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES Lexis)
endif()

if(BRAYNS_DEFLECT_ENABLED)
  list(APPEND BRAYNSPLUGINS_SOURCES extensions/plugins/DeflectPlugin.cpp)
  list(APPEND BRAYNSPLUGINS_PUBLIC_HEADERS extensions/plugins/DeflectPlugin.h)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES Deflect)
endif()

if(BRAYNS_NETWORKING_ENABLED)
  list(APPEND BRAYNSPLUGINS_SOURCES extensions/plugins/ZeroEQPlugin.cpp)
  list(APPEND BRAYNSPLUGINS_PUBLIC_HEADERS extensions/plugins/ZeroEQPlugin.h)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES
    PUBLIC ZeroEQ ZeroEQHTTP BraynsZeroBufRender ${LibJpegTurbo_LIBRARIES})
endif()

if(OSPRAY_FOUND)
  add_definitions( -DBRAYNS_USE_OSPRAY=1 )
  add_subdirectory(engines/ospray)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES PUBLIC braynsOSPRayEnginePlugin)
endif()

if(OPTIX_FOUND AND CUDA_FOUND)
  add_subdirectory(engines/optix)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES PUBLIC braynsOptiXEnginePlugin)
endif()

if(BRAYNS_LIVRE_ENABLED)
  add_subdirectory(engines/livre)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES PUBLIC braynsLivreEnginePlugin)
endif()

common_library(braynsPlugins)
