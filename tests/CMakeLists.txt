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
  list(APPEND EXCLUDE_FROM_TESTS brayns.cpp braynsTestData.cpp webAPI.cpp)
endif()
if(NOT BRAYNS_NETWORKING_ENABLED)
  list(APPEND EXCLUDE_FROM_TESTS webAPI.cpp)
endif()

include_directories(${PROJECT_SOURCE_DIR}/plugins/extensions/plugins)

list(APPEND LCOV_EXCLUDE 'plugins/extensions/plugins/base64/*'
                         'plugins/extensions/plugins/rapidjson/*'
                         'plugins/extensions/plugins/staticjson/*')
include(CommonCTest)
