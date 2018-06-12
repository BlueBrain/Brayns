# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(ExamplePlugin VERSION 1.0.0)

find_package(Brayns REQUIRED)

add_library(examplePlugin SHARED
  ExamplePlugin.cpp
  ExamplePlugin.h)

target_link_libraries(examplePlugin braynsPluginAPI)
