# Copyright (c) 2020, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(braynsFluorescence VERSION 1.0.0)
set(braynsFluorescence_VERSION_ABI 1)

set(BRAYNSFLUORESCENCE_HEADERS
  FluorescenceParams.h
  FluorescencePlugin.h
  json.hpp
  Log.h
)

set(BRAYNSFLUORESCENCE_SOURCES
  FluorescenceParams.cpp
  FluorescencePlugin.cpp
)

set(BRAYNSFLUORESCENCE_INCLUDE_DIR ${PROJECT_SOURCE_DIR})

add_library(braynsFluorescence SHARED ${BRAYNSFLUORESCENCE_SOURCES} ${BRAYNSFLUORESCENCE_HEADERS})
target_compile_options(braynsFluorescence PRIVATE "-Werror")
target_include_directories(braynsFluorescence PRIVATE ${BRAYNSFLUORESCENCE_INCLUDE_DIR})
target_link_libraries(braynsFluorescence PRIVATE braynsCommon braynsParameters braynsPluginAPI)

install(TARGETS braynsFluorescence
    EXPORT braynsFluorescence-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(EXPORT braynsFluorescence-targets
    DESTINATION share/plugins/braynsFluorescence/CMake
)
