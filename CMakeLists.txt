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
    set(BRAYNS_NETWORKING_ENABLED ON)
  endif()
  if(NOT DEFINED BRAYNS_DEFLECT_ENABLED)
    set(BRAYNS_DEFLECT_ENABLED ON)
  endif()
  if(NOT DEFINED BRAYNS_BRION_ENABLED)
    set(BRAYNS_BRION_ENABLED ON)
  endif()
endif()
include(Common)

# Core dependencies
common_find_package(Boost REQUIRED COMPONENTS
  filesystem system program_options unit_test_framework)
common_find_package(vmmlib REQUIRED)
common_find_package(OpenMP)
common_find_package(LibArchive)
common_find_package(libuv 1.8)

if(libuv_FOUND)
  # The libuv version of Ubuntu 16.04 is too old to have this definition that is
  # required by uvw (in fact, it uses an old enough version as well to make
  # things work, but couldn't go further).
  if(libuv_VERSION VERSION_LESS 1.9.0)
    add_definitions(-DUV_DISCONNECT=4)
  endif()
  if(NOT EXISTS ${PROJECT_SOURCE_DIR}/uvw)
    message(FATAL_ERROR "uvw missing, run: git submodule update --init")
  endif()
  include_directories(SYSTEM uvw/src)
  list(APPEND LCOV_EXCLUDE '${PROJECT_SOURCE_DIR}/uvw/*')
  # TODO: use tag v1.6.0_libuv-v1.18 for Docker
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build Async++ as a shared library" FORCE)
add_subdirectory(async++)
list(APPEND LCOV_EXCLUDE '${PROJECT_SOURCE_DIR}/async++/include/async++/*'
                         '${PROJECT_SOURCE_DIR}/async++/src/*')

# HTTP messaging
common_find_package(LibJpegTurbo)
common_find_package(Rockets)
if(ROCKETS_FOUND AND ROCKETS_USE_LIBWEBSOCKETS)
  option(BRAYNS_NETWORKING_ENABLED "Activate networking interfaces" ON)
  if(BRAYNS_NETWORKING_ENABLED)
    list(APPEND COMMON_FIND_PACKAGE_DEFINES BRAYNS_USE_NETWORKING)
  endif()
else()
  unset(BRAYNS_NETWORKING_ENABLED)
endif()

# Streaming to display walls
common_find_package(Deflect)
if(TARGET Deflect)
  option(BRAYNS_DEFLECT_ENABLED "Activate streaming to display wall" ON)
else()
  unset(BRAYNS_DEFLECT_ENABLED)
endif()

# Data access
common_find_package(Brion)
common_find_package(Servus)
common_find_package(HDF5 SYSTEM COMPONENTS C CXX)
if(TARGET Brion AND TARGET Servus AND HDF5_FOUND)
  option(BRAYNS_BRION_ENABLED "Activate Brion" ON)
else()
  unset(BRAYNS_BRION_ENABLED)
endif()

# Data access unit tests
common_find_package(BBPTestData)

if(NOT EXISTS ${PROJECT_SOURCE_DIR}/perceptualdiff/FindFreeImage.cmake)
  message(FATAL_ERROR "FindFreeImage.cmake missing, run: git submodule update --init")
endif()
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/perceptualdiff)
common_find_package(FreeImage)
if(FreeImage_FOUND)
  add_subdirectory(perceptualdiff)
endif()
list(APPEND LCOV_EXCLUDE '${PROJECT_SOURCE_DIR}/perceptualdiff/*')

# OSPRay rendering engine
common_find_package(ospray 1.5 SYSTEM)
set(_ospray_unsupported_version 1.6)
if(OSPRAY_FOUND AND OSPRAY_VERSION VERSION_LESS _ospray_unsupported_version)
  option(BRAYNS_OSPRAY_ENABLED "Activate OSPRay rendering engine" ON)
else()
  unset(BRAYNS_OSPRAY_ENABLED)
  if(OSPRAY_FOUND)
    message(WARNING "OSPRay version >= ${_ospray_unsupported_version} not supported")
  endif()
endif()

# Mesh loading
common_find_package(assimp SYSTEM)
if(ASSIMP_FOUND)
  option(BRAYNS_ASSIMP_ENABLED "Activate assimp library for standard 3D file format management" ON)
else()
  unset(BRAYNS_ASSIMP_ENABLED)
endif()

# Brayns viewer application
common_find_package(GLUT SYSTEM)
common_find_package(GLEW SYSTEM)
common_find_package(OpenGL)

if(GLUT_FOUND AND GLEW_FOUND AND OPENGL_FOUND)
  option(BRAYNS_VIEWER_ENABLED "Brayns Viewer" ON)
else()
  unset(BRAYNS_VIEWER_ENABLED)
endif()

common_find_package_post()

list(APPEND BRAYNS_DEPENDENT_LIBRARIES Async++ vmmlib)

# ------------------------------------------------------------------------------
# BRAYNS applications and libraries
# ------------------------------------------------------------------------------

if(NOT OSPRAY_FOUND)
  message(WARNING "OSPRay not found. Brayns cannot work without an engine" )
endif()

if(BRAYNS_OSPRAY_ENABLED)
  # ------------------------------------------------------------------------------
  # OSPRAY specific settings
  # ------------------------------------------------------------------------------
  # compiler selection for the non-MIC part
  if(APPLE)
    set(OSPRAY_COMPILER "CLANG" CACHE STRING "compiler to use (ICC,GCC, or CLANG)")
    set_property(CACHE OSPRAY_COMPILER PROPERTY STRINGS ICC GCC CLANG)
  else(APPLE)
    set(OSPRAY_COMPILER "GCC" CACHE STRING "compiler to use (ICC or GCC)")
    set_property(CACHE OSPRAY_COMPILER PROPERTY STRINGS ICC GCC)
  endif()
endif()

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
  unset(BRAYNS_SERVICE_ENABLED)
endif()

option(BRAYNS_BENCHMARK_ENABLED "Brayns Benchmark" ON)
if(BRAYNS_BENCHMARK_ENABLED)
  add_subdirectory(apps/BraynsBenchmark)
endif()

if(BRAYNS_OSPRAY_ENABLED)
  add_subdirectory(engines/ospray)
  add_subdirectory(ospray_modules)
endif()
add_subdirectory(plugins)
add_subdirectory(brayns)
add_subdirectory(tests)

include(CommonCPack)

set(DOXYGEN_MAINPAGE_MD README.md)
set(DOXYGEN_EXTRA_INPUT "${PROJECT_SOURCE_DIR}/README.md ${PROJECT_SOURCE_DIR}/Changelog.md")
set(COMMON_PROJECT_DOMAIN ch.epfl.bluebrain)
include(DoxygenRule)
