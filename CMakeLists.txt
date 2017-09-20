# Copyright (c) 2015-2017, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
set(Brayns_VERSION_ABI 1)
project(Brayns VERSION 0.3.0)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
                              ${CMAKE_SOURCE_DIR}/CMake/common)
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/CMake/common/Common.cmake)
  message(FATAL_ERROR "CMake/common missing, run: git submodule update --init")
endif()

set(COMMON_PROJECT_DOMAIN ch.epfl.bluebrain)
set(BRAYNS_DESCRIPTION "Hardware Agnostic Ray-Tracer")

# By default, optional dependencies are disabled
set(BRAYNS_USE_OPTIONAL_DEPENDENCY)
if($ENV{TRAVIS})
    # All optional dependencies are enabled for Continuous integration
  set(BRAYNS_USE_OPTIONAL_DEPENDENCY ON)
endif()

option(BRAYNS_BRION_ENABLED "Activate Brion" ${BRAYNS_USE_OPTIONAL_DEPENDENCY})
option(BRAYNS_DEFLECT_ENABLED "Activate streaming to display wall" ${BRAYNS_USE_OPTIONAL_DEPENDENCY})
option(BRAYNS_NETWORKING_ENABLED "Activate networking interfaces" ${BRAYNS_USE_OPTIONAL_DEPENDENCY})
option(BRAYNS_ASSIMP_ENABLED "Activate assimp library for standard 3D file format management" ON)
option(BRAYNS_IMAGEMAGICK_ENABLED "Activate IMAGEMAGICK library for standard image file format management" ON)
option(BRAYNS_OSPRAY_ENABLED "Activate OSPRay rendering engine" ON)
option(BRAYNS_OPTIX_ENABLED "Activate OptiX rendering engine" ON)
option(BRAYNS_LIVRE_ENABLED "Activate Livre rendering engine" ${BRAYNS_USE_OPTIONAL_DEPENDENCY})

set(EMBREE_MAX_ISA "AVX512KNL" CACHE STRING "MAX ISA SSE,AVX,AVX2,AVX512KNL")

include(Common)
include(BraynsDepResolver)

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

common_find_package(Boost REQUIRED COMPONENTS
  filesystem system program_options unit_test_framework)
common_find_package(vmmlib REQUIRED)
common_find_package(OpenMP)

if(BRAYNS_IMAGEMAGICK_ENABLED)
  common_find_package(Magick++ SYSTEM)
endif()

if( NOT( OPTIX_FOUND AND CUDA_FOUND ) AND NOT OSPRAY_FOUND AND NOT TARGET LivreLib )
  message( WARNING
      "Neither OptiX+CUDA, OSPRay nor Livre were found. Brayns cannot work without an engine" )
endif()

# ------------------------------------------------------------------------------
# BRAYNS applications and libraries
# ------------------------------------------------------------------------------

if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework OpenGL -Wno-narrowing -Wno-deprecated-declarations")
endif()

common_find_package_post()

add_subdirectory(plugins)
add_subdirectory(brayns)

option(BRAYNS_VIEWER_ENABLED "Brayns Viewer" ON)
if(BRAYNS_VIEWER_ENABLED)
  common_find_package(GLUT REQUIRED SYSTEM)
  common_find_package(GLEW REQUIRED SYSTEM)
  common_find_package(OpenGL REQUIRED)
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

add_subdirectory(tests)

include(DoxygenRule)
