# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSPLUGINS_LINK_LIBRARIES
  PRIVATE braynsCommon braynsIO vmmlib)

set(BRAYNSPLUGINS_SOURCES
  engines/EngineFactory.cpp
)

set(BRAYNSPLUGINS_PUBLIC_HEADERS
  extensions/plugins/ExtensionPlugin.h
  engines/EngineFactory.h
)

if(BRAYNS_DEFLECT_ENABLED OR BRAYNS_NETWORKING_ENABLED)
  list(APPEND BRAYNSPLUGINS_SOURCES extensions/ExtensionPluginFactory.cpp)
  list(APPEND BRAYNSPLUGINS_PUBLIC_HEADERS extensions/ExtensionPluginFactory.h)
endif()

if(BRAYNS_DEFLECT_ENABLED)
  list(APPEND BRAYNSPLUGINS_SOURCES extensions/plugins/DeflectPlugin.cpp)
  list(APPEND BRAYNSPLUGINS_PUBLIC_HEADERS extensions/plugins/DeflectPlugin.h)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES Deflect)
endif()

if(BRAYNS_NETWORKING_ENABLED)
  list(APPEND BRAYNSPLUGINS_HEADERS
    extensions/plugins/ImageGenerator.h
    extensions/plugins/jsonSerialization.h
    extensions/plugins/jsonUtils.h
  )
  list(APPEND BRAYNSPLUGINS_SOURCES
    extensions/plugins/ImageGenerator.cpp
    extensions/plugins/RocketsPlugin.cpp
    extensions/plugins/base64/base64.cpp
    extensions/plugins/staticjson/staticjson.cpp
  )
  set_source_files_properties(extensions/plugins/staticjson/staticjson.cpp
    PROPERTIES COMPILE_FLAGS -Wno-shadow)
  list(APPEND BRAYNSPLUGINS_PUBLIC_HEADERS extensions/plugins/RocketsPlugin.h)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES PUBLIC Rockets)
  if(LibJpegTurbo_FOUND)
    list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES PRIVATE ${LibJpegTurbo_LIBRARIES})
  endif()
  if(BRAYNS_IMAGEMAGICK_ENABLED)
    list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES PRIVATE ${Magick++_LIBRARIES})
  endif()
  if(libuv_FOUND)
    list(APPEND BRAYNSPLUGINS_HEADERS extensions/plugins/SocketListener.h)
    list(APPEND BRAYNSPLUGINS_SOURCES extensions/plugins/SocketListener.cpp)
  endif()
endif()

if(libuv_FOUND)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES PRIVATE ${libuv_LIBRARIES})
endif()

if(BRAYNS_OSPRAY_ENABLED)
  add_subdirectory(engines/ospray)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES PRIVATE braynsOSPRayEnginePlugin)
endif()

common_library(braynsPlugins)

# needed for staticjson and rapidjson
if(BRAYNS_NETWORKING_ENABLED)
  target_include_directories(braynsPlugins SYSTEM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extensions/plugins)
endif()
