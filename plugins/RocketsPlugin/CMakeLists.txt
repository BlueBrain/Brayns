# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(ROCKETSPLUGIN_HEADERS
  BinaryRequests.h
  ImageGenerator.h
  RocketsPlugin.h
  SnapshotTask.h
  jsonPropertyMap.h
  jsonSerialization.h
  jsonUtils.h
)

set(ROCKETSPLUGIN_SOURCES
  ImageGenerator.cpp
  RocketsPlugin.cpp
  staticjson/staticjson.cpp
)

set_source_files_properties(staticjson/staticjson.cpp
  PROPERTIES COMPILE_FLAGS "-Wno-shadow")

set(ROCKETSPLUGIN_LINK_LIBRARIES PRIVATE Rockets braynsParameters braynsTasks
  ${FREEIMAGE_LIBRARIES})
if(LibJpegTurbo_FOUND)
  list(APPEND ROCKETSPLUGIN_LINK_LIBRARIES PRIVATE ${LibJpegTurbo_LIBRARIES})
endif()

if(libuv_FOUND)
  list(APPEND ROCKETSPLUGIN_LINK_LIBRARIES PRIVATE ${libuv_LIBRARIES})
endif()

set(ROCKETSPLUGIN_OMIT_LIBRARY_HEADER ON)
set(ROCKETSPLUGIN_OMIT_VERSION_HEADERS ON)
set(ROCKETSPLUGIN_OMIT_EXPORT ON)
set(ROCKETSPLUGIN_INCLUDE_NAME rocketsplugin)
common_library(RocketsPlugin)
target_include_directories(RocketsPlugin SYSTEM PRIVATE ${FREEIMAGE_INCLUDE_DIRS})

# needed for staticjson and rapidjson
target_include_directories(RocketsPlugin SYSTEM PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include)
