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
set(CMAKE_FIND_PACKAGE_SORT_ORDER NATURAL)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Brayns VERSION 3.9.0)

# Extra CMake scripts
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)

# Options
option(BRAYNS_TESTS_ENABLED "Activate unit testing" ON)
option(BRAYNS_SERVICE_ENABLED "Activate Brayns Service" ON)
option(BRAYNS_CIRCUITEXPLORER_ENABLED "Activate CircuitExplorer plugin" OFF)
option(BRAYNS_DTI_ENABLED "Activate Diffusion-Transfer Imaging plugin" OFF)
option(BRAYNS_ATLASEXPLORER_ENABLED "Activate Atlas explorer plugin" OFF)
option(BRAYNS_CYLINDRICCAMERA_ENABLED "Activate Cylindric projection camera" OFF)
option(BRAYNS_MOLECULEEXPLORER_ENABLED "Activate MoleculeExplorer plugin" OFF)

# Core dependencies
find_package(Git REQUIRED)
find_package(OpenMP REQUIRED)
find_package(ospray 2.10 REQUIRED)
find_package(rkcommon 1.10 REQUIRED)
find_package(Poco QUIET COMPONENTS Foundation JSON Net NetSSL)
find_package(ZLIB REQUIRED)

# Set up version
include(GitUtils)
set(BRAYNS_GIT_HASH "Not found")
get_commit_hash(BRAYNS_GIT_HASH)
set(BRAYNS_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

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
