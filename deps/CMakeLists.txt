# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

add_library(stb INTERFACE)

target_include_directories(
    stb
    SYSTEM INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

add_library(tinyexr INTERFACE)

target_include_directories(
    tinyexr
    SYSTEM INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

add_library(doctest INTERFACE)

target_compile_definitions(doctest INTERFACE DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN)

target_include_directories(
    doctest
    SYSTEM INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)
