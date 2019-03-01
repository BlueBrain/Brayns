# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

if(NOT TARGET pdiff)
  return()
endif()

if(NOT TARGET BBPTestData)
  return()
endif()

if(NOT BRAYNS_OSPRAY_ENABLED OR NOT BRAYNS_ASSIMP_ENABLED)
  return()
endif()

configure_file(paths.h.in ${PROJECT_BINARY_DIR}/CircuitViewer/tests/paths.h)

set(TEST_LIBRARIES ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                   ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY}
                   BBPTestData brayns Brain braynsIO pdiff)

if(BRAYNS_NETWORKING_ENABLED)
  list(APPEND TEST_LIBRARIES Rockets braynsRockets)
  include_directories(
    ${PROJECT_SOURCE_DIR}/../Rockets
    ${PROJECT_SOURCE_DIR}/../Rockets/rapidjson/include
)
else()
  list(APPEND EXCLUDE_FROM_TESTS addCircuit.cpp)
endif()

if(NOT BRAYNS_OPTIX_TESTS_ENABLED)
    list(APPEND EXCLUDE_FROM_TESTS optixCircuit.cpp)
endif()

include(CommonCTest)

if(NOT TARGET Brayns-tests)
  add_custom_target(Brayns-tests)
endif()
add_dependencies(Brayns-tests BraynsCircuitViewer-tests)
