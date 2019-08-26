# Copyright (c) 2019, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(Streamer VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(FFMPEG REQUIRED QUIET)
find_package(ospray 1.8 REQUIRED QUIET)

list(APPEND CMAKE_MODULE_PATH ${OSPRAY_CMAKE_ROOT})
include(macros)
OSPRAY_CONFIGURE_MPI()

add_library(braynsStreamer SHARED streamer.cpp streamer.h)
target_include_directories(braynsStreamer PRIVATE ${FFMPEG_INCLUDE_DIR})
target_link_libraries(braynsStreamer
  PUBLIC braynsCommon braynsPluginAPI
  PRIVATE braynsEngine ${FFMPEG_LIBRARIES} NvPipe
          ${OSPRAY_COMMON_LIBRARY} ${MPI_LIBRARIES} ${OSPRAY_MPI_LIBRARIES}
)
set_target_properties(braynsStreamer PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION})

install(TARGETS braynsStreamer
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

if(TARGET Brayns-all)
  add_dependencies(Brayns-all braynsStreamer)
  add_dependencies(Brayns-install braynsStreamer)
endif()
