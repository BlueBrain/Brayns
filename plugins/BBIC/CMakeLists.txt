# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(BBICPlugin VERSION 1.0.0)

if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8 OR
   CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7)
  set(CMAKE_CXX_STANDARD 14)
else()
  set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(HighFive QUIET REQUIRED)
#find_package(HDF5 QUIET REQUIRED)
#find_package(Brayns QUIET REQUIRED)

set(BRAYNSBBIC_SOURCES
  BBICFile.cpp
  BBICLoader.cpp
  BBICPlugin.cpp
  VolumeModel.cpp
  lzfFilter/lzf/lzf_c.c
  lzfFilter/lzf/lzf_d.c
  lzfFilter/lzf_filter.c
)

set(BRAYNSBBIC_HEADERS
  BBICFile.h
  BBICLoader.h
  BBICPlugin.h
  VolumeModel.h
  lzfFilter/lzf_filter.h
)

add_library(braynsBBIC SHARED ${BRAYNSBBIC_SOURCES} ${BRAYNSBBIC_HEADERS})
target_link_libraries(braynsBBIC
  PUBLIC braynsCommon braynsPluginAPI
  PRIVATE braynsEngine ${HDF5_LIBRARIES} HighFive
)
target_include_directories(braynsBBIC PRIVATE ${HDF5_INCLUDE_DIRS})
set_target_properties(braynsBBIC PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION})

if(APPLE)
  target_compile_options(braynsBBIC PRIVATE "-Wno-expansion-to-defined")
endif()

install(TARGETS braynsBBIC
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

if(TARGET Brayns-all)
  add_dependencies(Brayns-all braynsBBIC)
  add_dependencies(Brayns-install braynsBBIC)
endif()
