# Copyright (c) 2016-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Daniel.Nachbaur@epfl.ch
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(TEST_LIBRARIES ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} brayns)

configure_file(paths.h.in ${PROJECT_BINARY_DIR}/tests/paths.h)
if(TARGET BBPTestData AND TARGET Lunchbox)
  list(APPEND TEST_LIBRARIES BBPTestData Lunchbox)
else()
  list(APPEND EXCLUDE_FROM_TESTS braynsTestData.cpp)
endif()
if(NOT BRAYNS_OSPRAY_ENABLED)
  list(APPEND EXCLUDE_FROM_TESTS brayns.cpp braynsTestData.cpp plugin.cpp webAPI.cpp)
endif()
if(BRAYNS_NETWORKING_ENABLED)
  add_library(myPlugin SHARED myPlugin.cpp)
  target_link_libraries(myPlugin brayns)
  target_include_directories(myPlugin SYSTEM PRIVATE ${PROJECT_SOURCE_DIR}/plugins/RocketsPlugin)
  list(APPEND EXCLUDE_FROM_TESTS myPlugin.cpp)
  list(APPEND TEST_LIBRARIES Rockets braynsPlugins myPlugin)
else()
  list(APPEND EXCLUDE_FROM_TESTS plugin.cpp webAPI.cpp myPlugin.cpp)
endif()

include_directories(${PROJECT_SOURCE_DIR}/plugins/RocketsPlugin)

list(APPEND LCOV_EXCLUDE 'plugins/RocketsPlugin/base64/*'
                         'plugins/RocketsPlugin/rapidjson/*'
                         'plugins/RocketsPlugin/staticjson/*')
include(CommonCTest)
