# Copyright (c) 2020, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(braynsPBRVolumes VERSION 0.1.0)
set(braynsPBRVolumes_VERSION_ABI 1)

set(BRAYNSPBRVOLUMES_HEADERS
  PBRVolumesParams.h
  PBRVolumesPlugin.h
  json.hpp
  Log.h
)

set(BRAYNSPBRVOLUMES_SOURCES
  PBRVolumesParams.cpp
  PBRVolumesPlugin.cpp
)

set(BRAYNSPBRVOLUMES_INCLUDE_DIR ${PROJECT_SOURCE_DIR})

add_library(braynsPBRVolumes SHARED ${BRAYNSPBRVOLUMES_SOURCES} ${BRAYNSPBRVOLUMES_HEADERS})
target_compile_options(braynsPBRVolumes PRIVATE "-Werror")
target_include_directories(braynsPBRVolumes PRIVATE ${BRAYNSPBRVOLUMES_INCLUDE_DIR})
target_link_libraries(braynsPBRVolumes PRIVATE braynsCommon braynsParameters braynsPluginAPI)

install(TARGETS braynsPBRVolumes
    EXPORT braynsPBRVolumes-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(EXPORT braynsPBRVolumes-targets
    DESTINATION share/plugins/braynsPBRVolumes/CMake
)
