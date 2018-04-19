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

set(BRAYNSPLUGINS_HEADERS
  engines/EngineFactory.h
)

if(BRAYNS_DEFLECT_ENABLED)
  list(APPEND BRAYNSPLUGINS_SOURCES DeflectPlugin/DeflectPlugin.cpp)
  list(APPEND BRAYNSPLUGINS_PUBLIC_HEADERS DeflectPlugin/DeflectPlugin.h)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES Deflect)
endif()

if(BRAYNS_NETWORKING_ENABLED)
  list(APPEND BRAYNSPLUGINS_HEADERS
    RocketsPlugin/BinaryRequests.h
    RocketsPlugin/ImageGenerator.h
    RocketsPlugin/jsonSerialization.h
    RocketsPlugin/jsonUtils.h
    RocketsPlugin/SnapshotTask.h
  )
  list(APPEND BRAYNSPLUGINS_SOURCES
    RocketsPlugin/ImageGenerator.cpp
    RocketsPlugin/RocketsPlugin.cpp
    RocketsPlugin/base64/base64.cpp
    RocketsPlugin/staticjson/staticjson.cpp
  )
  set_source_files_properties(RocketsPlugin/staticjson/staticjson.cpp
    PROPERTIES COMPILE_FLAGS -Wno-shadow)
  list(APPEND BRAYNSPLUGINS_PUBLIC_HEADERS RocketsPlugin/RocketsPlugin.h)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES PRIVATE Rockets braynsParameters)
  if(LibJpegTurbo_FOUND)
    list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES PRIVATE ${LibJpegTurbo_LIBRARIES})
  endif()
  if(BRAYNS_IMAGEMAGICK_ENABLED)
    list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES PRIVATE ${Magick++_LIBRARIES})
  endif()
endif()

if(libuv_FOUND)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES PRIVATE ${libuv_LIBRARIES})
endif()

if(BRAYNS_OSPRAY_ENABLED)
  add_subdirectory(engines/ospray)
  list(APPEND BRAYNSPLUGINS_LINK_LIBRARIES PRIVATE braynsOSPRayEnginePlugin)
endif()

if(CPPCHECK_VERSION VERSION_LESS 1.78)
  list(APPEND CPPCHECK_EXTRA_ARGS --error-exitcode=0 --force)
endif()

set(BRAYNSPLUGINS_OMIT_LIBRARY_HEADER ON)
set(BRAYNSPLUGINS_OMIT_VERSION_HEADERS ON)
set(BRAYNSPLUGINS_INCLUDE_NAME plugins)
common_library(braynsPlugins)

# needed for staticjson and rapidjson
if(BRAYNS_NETWORKING_ENABLED)
  target_include_directories(braynsPlugins SYSTEM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/RocketsPlugin)
endif()
