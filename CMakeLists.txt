# Copyright (c) 2015-2018, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#                     Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(Brayns VERSION 0.8.0)
set(Brayns_VERSION_ABI 1)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
                              ${CMAKE_SOURCE_DIR}/CMake/common)
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/CMake/common/Common.cmake)
  message(FATAL_ERROR "CMake/common missing, run: git submodule update --init")
endif()

set(BRAYNS_LICENSE LGPL)
set(BRAYNS_DESCRIPTION "Hardware Agnostic Ray-Tracer")
set(BRAYNS_PORT_DEPEND "assimp boost freeimage glew tbb")

# Enable cloning of all optional subprojects for CI builds
if($ENV{TRAVIS})
  if(NOT DEFINED BRAYNS_NETWORKING_ENABLED)
    option(BRAYNS_NETWORKING_ENABLED "Activate networking interfaces" ON)
  endif()
  if(NOT DEFINED BRAYNS_DEFLECT_ENABLED)
    option(BRAYNS_DEFLECT_ENABLED "Activate streaming to display wall" ON)
  endif()
  if(NOT DEFINED BRAYNS_CIRCUITVIEWER_ENABLED)
    option(BRAYNS_CIRCUITVIEWER_ENABLED "Activate CircuitViewer plugin" ON)
  endif()
  if(NOT DEFINED BRAYNS_BBIC_ENABLED)
    option(BRAYNS_BBIC_ENABLED "Activate BBIC Volume plugin" ON)
  endif()
endif()

set(CHOOSE_PYTHON_IGNORE_BOOST ON)
include(Common)

# Core dependencies
common_find_package(Boost REQUIRED COMPONENTS
  filesystem system program_options unit_test_framework)
common_find_package(OpenMP)
common_find_package(libuv 1.8)
common_find_package(FreeImage)

# Archive loading
common_find_package(LibArchive)
option(BRAYNS_LIBARCHIVE_ENABLED "Activate libarchive for archive loading" ${LibArchive_FOUND})
if(BRAYNS_LIBARCHIVE_ENABLED)
  common_find_package(LibArchive REQUIRED)
else()
  common_find_package_disable(LibArchive)
endif()

# HTTP messaging
common_find_package(LibJpegTurbo)
common_find_package(Rockets)
if(ROCKETS_FOUND AND ROCKETS_USE_LIBWEBSOCKETS)
  option(BRAYNS_NETWORKING_ENABLED "Activate networking interfaces" ON)
  if(BRAYNS_NETWORKING_ENABLED)
    list(APPEND COMMON_FIND_PACKAGE_DEFINES BRAYNS_USE_NETWORKING)
  endif()
else()
  set(BRAYNS_NETWORKING_ENABLED OFF)
endif()

# OSPRay rendering engine
common_find_package(ospray 1.8 SYSTEM)
option(BRAYNS_OSPRAY_ENABLED "Activate OSPRay rendering engine" ${OSPRAY_FOUND})
if(BRAYNS_OSPRAY_ENABLED)
  common_find_package(ospray 1.8 SYSTEM REQUIRED)
else()
  common_find_package_disable(ospray)
endif()

option(BRAYNS_OPTIX_ENABLED "Activate OptiX rendering engine" OFF)
option(BRAYNS_OPTIX_TESTS_ENABLED "Activate OptiX tests" OFF)
if(BRAYNS_OPTIX_ENABLED)
  # We have to append OptiX ourselves since we do not use common_find_package()
  list(APPEND COMMON_FIND_PACKAGE_DEFINES BRAYNS_USE_OPTIX)
  add_subdirectory(engines/optix)
endif()

# Mesh loading
common_find_package(assimp SYSTEM)
option(BRAYNS_ASSIMP_ENABLED "Activate assimp library for standard 3D file format management" ${assimp_FOUND})
if(BRAYNS_ASSIMP_ENABLED)
  common_find_package(assimp SYSTEM REQUIRED)
else()
  common_find_package_disable(assimp)
endif()

# Brayns viewer application
common_find_package(GLEW SYSTEM)
common_find_package(OpenGL)

if(GLEW_FOUND AND OPENGL_FOUND)
  option(BRAYNS_VIEWER_ENABLED "Activate Brayns Viewer" ON)
else()
  option(BRAYNS_VIEWER_ENABLED "Activate Brayns Viewer" OFF)
endif()
if(BRAYNS_VIEWER_ENABLED)
    common_find_package(GLEW SYSTEM REQUIRED)
    common_find_package(OpenGL REQUIRED)
else()
    common_find_package_disable(GLEW)
    common_find_package_disable(OpenGL)
endif()

common_find_package_post()

list(APPEND BRAYNS_DEPENDENT_LIBRARIES Async++ glm)

add_subdirectory(deps)

if(libuv_FOUND)
  # The libuv version of Ubuntu 16.04 is too old to have this definition that is
  # required by uvw (in fact, it uses an old enough version as well to make
  # things work, but couldn't go further).
  if(libuv_VERSION VERSION_LESS 1.9.0)
    add_definitions(-DUV_DISCONNECT=4)
  endif()
  include_directories(SYSTEM deps/uvw/src)
endif()

# ------------------------------------------------------------------------------
# BRAYNS applications and libraries
# ------------------------------------------------------------------------------

if(BRAYNS_VIEWER_ENABLED)
  add_subdirectory(apps/ui)
  add_subdirectory(apps/BraynsViewer)
endif()

if(libuv_FOUND)
  option(BRAYNS_SERVICE_ENABLED "Brayns Service" ON)
  if(BRAYNS_SERVICE_ENABLED)
    add_subdirectory(apps/BraynsService)
  endif()
else()
  set(BRAYNS_SERVICE_ENABLED OFF)
endif()

option(BRAYNS_BENCHMARK_ENABLED "Brayns Benchmark" ON)
if(BRAYNS_BENCHMARK_ENABLED)
  add_subdirectory(apps/BraynsBenchmark)
endif()

if(BRAYNS_OSPRAY_ENABLED)
  add_subdirectory(engines/ospray)
else()
  message(WARNING "OSPRay not found or enabled. Brayns cannot work without an engine" )
endif()
add_subdirectory(plugins)
add_subdirectory(brayns)
add_subdirectory(tests)

include(CommonCPack)

set(DOXYGEN_MAINPAGE_MD README.md)
set(DOXYGEN_EXTRA_INPUT "${PROJECT_SOURCE_DIR}/README.md ${PROJECT_SOURCE_DIR}/Changelog.md")
set(COMMON_PROJECT_DOMAIN ch.epfl.bluebrain)
include(DoxygenRule)
