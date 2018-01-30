# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(Brayns VERSION 0.4.0)
set(Brayns_VERSION_ABI 1)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
                              ${CMAKE_SOURCE_DIR}/CMake/common)
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/CMake/common/Common.cmake)
  message(FATAL_ERROR "CMake/common missing, run: git submodule update --init")
endif()

set(BRAYNS_DESCRIPTION "Hardware Agnostic Ray-Tracer")
set(BRAYNS_PORT_DEPEND "assimp boost ImageMagick glew tbb")

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

# HTTP messaging
common_find_package(LibJpegTurbo)
common_find_package(Rockets)
if(LibJpegTurbo_FOUND AND TARGET Rockets)
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
common_find_package(Lunchbox)

# OSPRay rendering engine
common_find_package(ospray SYSTEM)
if(OSPRAY_FOUND)
  option(BRAYNS_OSPRAY_ENABLED "Activate OSPRay rendering engine" ON)
else()
  unset(BRAYNS_OSPRAY_ENABLED)
endif()

# Image processing
common_find_package(Magick++ SYSTEM)
if(Magick++_FOUND)
  option(BRAYNS_IMAGEMAGICK_ENABLED "Activate IMAGEMAGICK library for standard image file format management" ON)
else()
  unset(BRAYNS_IMAGEMAGICK_ENABLED)
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

if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework OpenGL -Wno-narrowing -Wno-deprecated-declarations")
endif()

if(BRAYNS_VIEWER_ENABLED)
  add_subdirectory(apps/ui)
  add_subdirectory(apps/BraynsViewer)
endif()

option(BRAYNS_SERVICE_ENABLED "Brayns Service" ON)
if(BRAYNS_SERVICE_ENABLED)
  add_subdirectory(apps/BraynsService)
endif()

option(BRAYNS_BENCHMARK_ENABLED "Brayns Benchmark" ON)
if(BRAYNS_BENCHMARK_ENABLED)
  add_subdirectory(apps/BraynsBenchmark)
endif()

add_subdirectory(plugins)
add_subdirectory(brayns)
add_subdirectory(tests)

# OptiX module
if(BRAYNS_OSPRAY_ENABLED)
  # Needs OSPRay version with a fix, so enable on your risk
  # https://github.com/ospray/ospray/commit/851b3e52120982470276ce2cbc04b5de654ddbde
  if(OSPRAY_VERSION VERSION_LESS 1.5.0)
    option(BRAYNS_OPTIX_ENABLED "Activate OptiX module" OFF)
  else()
    option(BRAYNS_OPTIX_ENABLED "Activate OptiX module" ON)
  endif()
  if(BRAYNS_OPTIX_ENABLED)
    add_subdirectory(ospray_modules/optix)
    if(TARGET ospray_module_optix)
      add_dependencies(braynsOSPRayEnginePlugin ospray_module_optix)
    endif()
  endif()
endif()

set(COMMON_PROJECT_DOMAIN ch.epfl.bluebrain)
include(DoxygenRule)
