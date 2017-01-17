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
include(GitExternal)

set(COMMON_PROJECT_DOMAIN ch.epfl.bluebrain)
set(BRAYNS_DESCRIPTION "Hardware Agnostic Ray-Tracer")

option(BRAYNS_LIVRE_ENABLED "Activate Livre rendering engine" ON)

include(Common)

# Make Brayns optional when OSPRay is not available and:
# - building as a subproject
# - building on Ubuntu Jenkins VM
if(NOT ${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR} OR
        (TRAVIS AND LSB_DISTRIBUTOR_ID MATCHES "Ubuntu"))
  common_find_package(OSPRay)
  if(NOT OSPRAY_FOUND)
      message(WARNING "[Brayns] OSPRay not found, the project will not be built")
    include(CommonPackageConfig)
    return()
  endif()
endif()

common_find_package(Boost REQUIRED COMPONENTS program_options unit_test_framework)
common_find_package(vmmlib REQUIRED)
common_find_package(OptiX SYSTEM)
common_find_package(CUDA SYSTEM)
common_find_package(OSPRay SYSTEM)
common_find_package(OpenMP)
common_find_package(Lunchbox)

if( NOT( OPTIX_FOUND AND CUDA_FOUND ) AND NOT OSPRAY_FOUND AND NOT TARGET LivreLib )
    message( FATAL_ERROR
        "Neither OptiX+CUDA, OSPRay nor Livre were found. Brayns cannot work without an engine" )
endif()

option(BRAYNS_IMAGEMAGICK_ENABLED "Activate IMAGEMAGICK library for standard image file format management" ON)
if(BRAYNS_IMAGEMAGICK_ENABLED)
  common_find_package(Magick++ SYSTEM)
endif()

option(BRAYNS_ASSIMP_ENABLED "Activate assimp library for standard 3D file format management" ON)
if(BRAYNS_ASSIMP_ENABLED)
  common_find_package(assimp)
endif()

option(BRAYNS_BRION_ENABLED "Activate Brion" ON)
if(BRAYNS_BRION_ENABLED)
  common_find_package(BBPTestData)
  common_find_package(Brion)
  common_find_package(HDF5 SYSTEM REQUIRED COMPONENTS C CXX)
endif()

option(BRAYNS_DEFLECT_ENABLED "Activate streaming to display wall" ON)
if(BRAYNS_DEFLECT_ENABLED)
  common_find_package(Deflect)
endif()

option(BRAYNS_REST_ENABLED "Activate REST" ON)
if(BRAYNS_REST_ENABLED)
  common_find_package(Servus REQUIRED)
  common_find_package(ZeroBuf REQUIRED)
  common_find_package(Lexis REQUIRED)
  common_find_package(LibJpegTurbo REQUIRED)
  common_find_package(ZeroEQ REQUIRED)
endif()

common_find_package_post()

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

# the arch we're targeting for the non-MIC/non-xeon phi part of ospray
set(BRAYNS_BUILD_ISA "ALL" CACHE STRING "Target ISA (SSE,AVX,AVX2,or ALL)")
set_property(CACHE BRAYNS_BUILD_ISA PROPERTY STRINGS ALL SSE AVX AVX2)

# ------------------------------------------------------------------------------
# BRAYNS applications and libraries
# ------------------------------------------------------------------------------

if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework OpenGL -Wno-narrowing -Wno-deprecated-declarations")
endif()

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

option(BRAYNS_BENCHMARK_ENABLED "Brayns Benchmark" OFF)
if(BRAYNS_BENCHMARK_ENABLED)
  add_subdirectory(apps/BraynsBenchmark)
endif()

add_subdirectory(tests)

include(DoxygenRule)
