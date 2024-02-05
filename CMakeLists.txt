# Copyright 2015-2024 Blue Brain Project/EPFL
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#                     Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
#                     Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# CMake configuration
set(CMAKE_FIND_PACKAGE_SORT_ORDER NATURAL)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Brayns VERSION 2.0.0)

# Extra CMake scripts
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)

# Options
option(BRAYNS_TESTS_ENABLED "Activate unit testing" ON)
option(BRAYNS_VIEWER_ENABLED "Activate Brayns Viewer" OFF)
option(BRAYNS_SERVICE_ENABLED "Activate Brayns Service" ON)
option(BRAYNS_BENCHMARK_ENABLED "Activate Brayns Benchmark" OFF)
option(BRAYNS_IBL_ENABLED "Activate Brayns IBL tool" OFF)
option(BRAYNS_DEFLECT_ENABLED "Activate streaming to display wall" OFF)
option(BRAYNS_CIRCUITEXPLORER_ENABLED "Activate CircuitExplorer plugin" ON)
option(BRAYNS_CIRCUITINFO_ENABLED "Activate CircuitInfo plugin" OFF)
option(BRAYNS_DTI_ENABLED "Activate Diffusion-Transfer Imaging plugin" OFF)
option(BRAYNS_OPENDECK_ENABLED "Activate OpenDeck plugin" OFF)
option(BRAYNS_MULTIVIEW_ENABLED "Activate Multiview camera plugin" OFF)
option(BRAYNS_HARDWARE_RANDOMIZER_ENABLED "Enables hardware randomizer if supported by target CPU" OFF)

# Core dependencies
find_package(Git REQUIRED)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(OpenMP REQUIRED)
find_package(ospray 1.8 REQUIRED)
find_package(assimp REQUIRED)
find_package(Poco QUIET COMPONENTS Foundation JSON Net NetSSL)
find_package(glm QUIET)

# Set up version
include(GitUtils)
set(BRAYNS_GIT_HASH "Not found")
get_commit_hash(BRAYNS_GIT_HASH)
set(BRAYNS_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

# Brayns targets compile options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(BRAYNS_COMPILE_OPTIONS -Wall -Wextra -pedantic -Werror)

# Downloadable dependencies
add_subdirectory(deps)

# BRAYNS core
add_subdirectory(brayns)

# BRAYNS apps
add_subdirectory(apps)

# BRAYNS OSPRay render engine
add_subdirectory(engines/ospray)

# BRAYNS plugins
add_subdirectory(plugins)

# BRAYNS tests
if(BRAYNS_TESTS_ENABLED)
  add_subdirectory(tests)
endif()

# BRAYNS doc
add_subdirectory(doc)
