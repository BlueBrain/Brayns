# Copyright (c) HBP 2014-2015 cyrille.favreau@epfl.ch
# All rights reserved. Do not distribute without further notice.

cmake_minimum_required(VERSION 2.8)

project(BRayns)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
  ${CMAKE_SOURCE_DIR}/CMake/common)
include(GitExternal)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "1")
set(VERSION_PATCH "0")
set(VERSION_ABI 1)

set(BRAYNS_DESCRIPTION "Ray-tracing engine")

include(Common)

# Make BRayns optional when OSPRay is not available and:
# - building as a subproject
# - building on Ubuntu Jenkins VM
if(NOT ${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR} OR
        (TRAVIS AND LSB_DISTRIBUTOR_ID MATCHES "Ubuntu"))
  common_package(OSPRay)
  if(NOT OSPRAY_FOUND)
      message(WARNING "[BRayns] OSPRay not found, the project will not be built")
    include(CommonPackageConfig)
    return()
  endif()
endif()

if(APPLE)
  cmake_policy(SET CMP0042 NEW)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.10" CACHE STRING "Deployment target for OSX" FORCE)
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "OSX Architectures" FORCE)
endif()

common_package(Boost REQUIRED COMPONENTS program_options)
common_package(OSPRay REQUIRED SYSTEM)
common_package(GLUT REQUIRED SYSTEM)
common_package(OpenGL REQUIRED)
common_package(vmmlib REQUIRED)

option(IMAGEMAGICK_ENABLED "Activate IMAGEMAGICK library for standard image file format management" ON)
if(IMAGEMAGICK_ENABLED)
  common_package(Magick++)
endif()

option(ASSIMP_ENABLED "Activate assimp library for standard 3D file format management" ON)
if(ASSIMP_ENABLED)
  common_package(assimp)
endif()

option(BBPSDK_ENABLED "Activate BBPSDK" ON)
if(BBPSDK_ENABLED)
  common_package(BBPSDK)
endif()

option(DEFLECT_ENABLED "Activate streaming to display wall" ON)
if(DEFLECT_ENABLED)
  common_package(Deflect)
endif()

option(REST_ENABLED "Activate REST" ON)
if(REST_ENABLED)
  common_package(FlatBuffers)
  common_package(ZeroBuf)
  common_package(LibJpegTurbo)
  common_package(zeq)
endif()

common_package_post()

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

add_subdirectory(plugins)
add_subdirectory(brayns)

option(BRAYNSVIEWER_ENABLED "BRayns Viewer" ON)
if(BRAYNSVIEWER_ENABLED)
  add_subdirectory(apps/BRaynsViewer)
endif()

option(BRAYNSSERVICE_ENABLED "BRayns Service" ON)
if(BRAYNSSERVICE_ENABLED)
  add_subdirectory(apps/BRaynsService)
endif()

include(DoxygenRule)
