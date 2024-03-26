# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Authors:
# Cyrille Favreau <cyrille.favreau@epfl.ch>
# Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
# Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# CMake configuration
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Brayns VERSION 3.7.0)

# Options
option(BRAYNS_TESTS_ENABLED "Activate unit testing" ON)
option(BRAYNS_SERVICE_ENABLED "Activate Brayns Service" ON)
option(BRAYNS_CIRCUITEXPLORER_ENABLED "Activate CircuitExplorer plugin" OFF)
option(BRAYNS_ATLASEXPLORER_ENABLED "Activate Atlas explorer plugin" OFF)

# Core dependencies
find_package(OpenMP REQUIRED)
find_package(ospray REQUIRED)
find_package(rkcommon REQUIRED)
find_package(Poco QUIET COMPONENTS Foundation JSON Net NetSSL)
find_package(ZLIB REQUIRED)

# Set up version
set(BRAYNS_PRE_RELEASE "")
set(BRAYNS_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

if(NOT ${BRAYNS_PRE_RELEASE} EQUAL "")
    set(BRAYNS_VERSION "${BRAYNS_VERSION}-${BRAYNS_PRE_RELEASE}")
endif()

# Brayns targets compile options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(BRAYNS_COMPILE_OPTIONS -Wall -Wextra -Wconversion -pedantic-errors -Werror)

# Downloadable dependencies
add_subdirectory(deps)

# BRAYNS core
add_subdirectory(brayns)

# BRAYNS apps
add_subdirectory(apps)

# BRAYNS plugins
add_subdirectory(plugins)

# BRAYNS tests
if(BRAYNS_TESTS_ENABLED)
    add_subdirectory(tests)
endif()
