# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(ExampleBraynsPlugin VERSION 1.0.0)

# if not superproject aka part of Brayns, we need to find it first
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  find_package(Brayns REQUIRED)
endif()

add_library(exampleBraynsPlugin SHARED
  Plugin.cpp
  Plugin.h)

target_link_libraries(
  exampleBraynsPlugin
  braynsPluginAPI
  braynsNetwork
  braynsCommon
)
