# Copyright (c) 2017, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
#
# This file is part of https://github.com/BlueBrain/ospray-modules

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(ospray_module_optix VERSION 1.0.0)
set(ospray_module_optix_VERSION_ABI 1)

include(Common)

# For FindOptix and CUDA patch
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

common_find_package(CUDA REQUIRED SYSTEM)

# Patch for cmake 3.7->3.9
if(CMAKE_VERSION VERSION_GREATER "3.6" AND CMAKE_VERSION VERSION_LESS "3.10")
  set(CUDA_run_nvcc "${CMAKE_CURRENT_SOURCE_DIR}/CMake/run_nvcc.cmake")
endif()

common_find_package(OpenMP)
common_find_package(OptiX REQUIRED SYSTEM)
common_find_package(ospray REQUIRED SYSTEM)
common_find_package_post()

set(OSPRAY_MODULE_OPTIX_OMIT_LIBRARY_HEADER ON)
set(OSPRAY_MODULE_OPTIX_OMIT_VERSION_HEADERS ON)

set(OSPRAY_MODULE_OPTIX_CU
  cuda/camera/Camera.cu
  cuda/Constantbg.cu
  cuda/geometry/Cones.cu
  cuda/geometry/Cylinders.cu
  cuda/geometry/Spheres.cu
  cuda/geometry/TrianglesMesh.cu
  cuda/renderer/Phong.cu
)
CUDA_GET_SOURCES_AND_OPTIONS(source_files cmake_options options ${ARGN})
CUDA_WRAP_SRCS(ospray_module_optix
  PTX generated_files ${OSPRAY_MODULE_OPTIX_CU} ${cmake_options}
  OPTIONS ${options}
)


set(OSPRAY_MODULE_OPTIX_PTX
  ospray_module_optix_generated_Camera.cu.ptx
  ospray_module_optix_generated_Constantbg.cu.ptx
  ospray_module_optix_generated_Cones.cu.ptx
  ospray_module_optix_generated_Cylinders.cu.ptx
  ospray_module_optix_generated_Spheres.cu.ptx
  ospray_module_optix_generated_TrianglesMesh.cu.ptx
  ospray_module_optix_generated_Phong.cu.ptx
)
include(StringifyPtx)
stringify_ptx(${OSPRAY_MODULE_OPTIX_PTX})


set(OSPRAY_MODULE_OPTIX_HEADERS
  camera/Camera.h
  Context.h
  cuda/CommonStructs.h
  cuda/geometry/IntersectionRefinement.h
  cuda/Helpers.h
  cuda/Random.h
  cuda/renderer/Phong.h
  Device.h
  framebuffer/FrameBuffer.h
  geom/Cones.h
  geom/Cylinders.h
  geom/Geometry.h
  geom/Spheres.h
  geom/TriangleMesh.h
  geom/Instance.h
  lights/DirectionalLight.h
  lights/Light.h
  lights/PointLight.h
  Model.h
  ${PTX_HEADERS}
  render/Material.h
  render/Renderer.h
)

set(OSPRAY_MODULE_OPTIX_SOURCES
  camera/Camera.cpp
  Context.cpp
  Device.cpp
  framebuffer/FrameBuffer.cpp
  geom/Cones.cpp
  geom/Cylinders.cpp
  geom/Geometry.cpp
  geom/Spheres.cpp
  geom/TriangleMesh.cpp
  geom/Instance.cpp
  lights/DirectionalLight.cpp
  lights/Light.cpp
  lights/PointLight.cpp
  Model.cpp
  ${PTX_SOURCES}
  render/Material.cpp
  render/Renderer.cpp
)

set(OSPRAY_MODULE_OPTIX_LINK_LIBRARIES PRIVATE ${OptiX_LIBRARIES})
common_library(ospray_module_optix)

set(DOXYGEN_MAINPAGE_MD README.md)
set(DOXYGEN_EXTRA_INPUT "${PROJECT_SOURCE_DIR}/README.md")
set(COMMON_PROJECT_DOMAIN ch.epfl.bluebrain)
include(DoxygenRule)
