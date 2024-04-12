# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Authors: Daniel.Nachbaur@epfl.ch
# Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
# Adrien Fleury <adrien.fleury@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

file(GLOB_RECURSE TEST_SOURCES core/Test*.cpp)

foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WLE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
    target_include_directories(${TEST_NAME} SYSTEM PRIVATE deps)
    target_link_libraries(${TEST_NAME} PRIVATE brayns::core)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endforeach()
