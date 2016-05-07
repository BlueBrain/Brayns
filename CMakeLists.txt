# Copyright (c) 2011-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of BRayns

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
set(BRayns_VERSION_ABI 1)
project(BRayns VERSION 0.1.0)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
                              ${CMAKE_SOURCE_DIR}/CMake/common)
include(GitExternal)

set(BRAYNS_DESCRIPTION "Ray-tracing engine")

include(Common)

# Make BRayns optional when OSPRay is not available and:
# - building as a subproject
# - building on Ubuntu Jenkins VM
if(NOT ${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR} OR
        (TRAVIS AND LSB_DISTRIBUTOR_ID MATCHES "Ubuntu"))
  common_find_package(OSPRay)
  if(NOT OSPRAY_FOUND)
      message(WARNING "[BRayns] OSPRay not found, the project will not be built")
    include(CommonPackageConfig)
    return()
  endif()
endif()

common_find_package(Boost REQUIRED COMPONENTS program_options)
common_find_package(OSPRay REQUIRED SYSTEM)
common_find_package(GLUT REQUIRED SYSTEM)
common_find_package(OpenGL REQUIRED)
common_find_package(vmmlib REQUIRED)

option(BRAYNS_IMAGEMAGICK_ENABLED "Activate IMAGEMAGICK library for standard image file format management" ON)
if(BRAYNS_IMAGEMAGICK_ENABLED)
  common_find_package(Magick++)
endif()

option(BRAYNS_ASSIMP_ENABLED "Activate assimp library for standard 3D file format management" ON)
if(BRAYNS_ASSIMP_ENABLED)
  common_find_package(assimp)
endif()

option(BRAYNS_BRION_ENABLED "Activate Brion" ON)
if(BRAYNS_BRION_ENABLED)
  common_find_package(Brion)
endif()

option(BRAYNS_DEFLECT_ENABLED "Activate streaming to display wall" ON)
if(BRAYNS_DEFLECT_ENABLED)
  common_find_package(Deflect)
endif()

option(BRAYNS_REST_ENABLED "Activate REST" ON)
if(BRAYNS_REST_ENABLED)
  common_find_package(Servus REQUIRED)
  common_find_package(ZeroBuf REQUIRED)
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
add_subdirectory(apps/ui)

option(BRAYNS_VIEWER_ENABLED "BRayns Viewer" ON)
if(BRAYNS_VIEWER_ENABLED)
  add_subdirectory(apps/BRaynsViewer)
endif()

option(BRAYNS_SERVICE_ENABLED "BRayns Service" ON)
if(BRAYNS_SERVICE_ENABLED)
  add_subdirectory(apps/BRaynsService)
endif()

include(DoxygenRule)
