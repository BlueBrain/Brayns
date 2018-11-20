# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(BraynsOpenDeck VERSION 1.0.0)
set(BraynsOpenDeck_VERSION_ABI 1)

include(Common)

set(BRAYNSOPENDECK_HEADERS OpenDeckPlugin.h)
set(BRAYNSOPENDECK_SOURCES OpenDeckPlugin.cpp)
set(BRAYNSOPENDECK_LINK_LIBRARIES PRIVATE braynsCommon braynsPluginAPI)

set(BRAYNSOPENDECK_OMIT_LIBRARY_HEADER ON)
set(BRAYNSOPENDECK_OMIT_VERSION_HEADERS ON)
set(BRAYNSOPENDECK_OMIT_EXPORT ON)
set(BRAYNSOPENDECK_INCLUDE_NAME brayns_opendeck)
common_library(braynsOpenDeck)

if (TARGET Brayns-all)
  add_dependencies(Brayns-all braynsOpenDeck)
endif()
