# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(BraynsCircuitViewer VERSION 1.0.0)
set(BraynsCircuitViewer_VERSION_ABI 1)

include(Common)

common_find_package(Brion REQUIRED)
# For unit tests
common_find_package(BBPTestData)
common_find_package_post()

set(BRAYNSCIRCUITVIEWER_HEADERS
  CircuitViewer.h
  io/common.h
  io/CircuitLoader.h
  io/MorphologyLoader.h
  io/SimulationHandler.h)

set(BRAYNSCIRCUITVIEWER_SOURCES
  CircuitViewer.cpp
  io/CircuitLoader.cpp
  io/MorphologyLoader.cpp
  io/SimulationHandler.cpp)

set(BRAYNSCIRCUITVIEWER_LINK_LIBRARIES PRIVATE braynsPluginAPI Brain)

set(BRAYNSCIRCUITVIEWER_OMIT_LIBRARY_HEADER ON)
set(BRAYNSCIRCUITVIEWER_OMIT_VERSION_HEADERS ON)
set(BRAYNSCIRCUITVIEWER_OMIT_EXPORT ON)
common_library(braynsCircuitViewer)

add_subdirectory(tests)

if (TARGET Brayns-all)
  add_dependencies(Brayns-all BraynsCircuitViewer-all)
  add_dependencies(Brayns-install BraynsCircuitViewer-install)
endif()
