# Copyright (c) 2019, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(braynsCircuitInfo VERSION 0.1.0)
set(braynsCircuitInfo_VERSION_ABI 1)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
                              ${CMAKE_SOURCE_DIR}/CMake/common)
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/CMake/common/Common.cmake)
  message(FATAL_ERROR "CMake/common missing, run: git submodule update --init --recursive")
endif()

include(Common)
common_find_package(Brion)
common_find_package_post()


set(BRAYNSCIRCUITINFO_HEADERS
  CircuitInfoParams.h
  CircuitInfoPlugin.h
  Log.h
)

set(BRAYNSCIRCUITINFO_SOURCES
  CircuitInfoPlugin.cpp
)

set(BRAYNSCIRCUITINFO_LINK_LIBRARIES PRIVATE Brion Brain braynsCommon braynsParameters braynsPluginAPI)

set(BRAYNSCIRCUITINFO_OMIT_LIBRARY_HEADER ON)
set(BRAYNSCIRCUITINFO_OMIT_VERSION_HEADERS ON)
set(BRAYNSCIRCUITINFO_OMIT_EXPORT ON)

common_library(braynsCircuitInfo)
