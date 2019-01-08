# Copyright (c) 2015-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

# For FindOptix and CUDA patch
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

common_find_package(CUDA REQUIRED SYSTEM)
message(STATUS "Using CUDA version ${CUDA_VERSION}")

# Patch for cmake 3.7->3.9
if(CMAKE_VERSION VERSION_GREATER "3.6" AND CMAKE_VERSION VERSION_LESS "3.10")
  set(CUDA_run_nvcc "${CMAKE_CURRENT_SOURCE_DIR}/CMake/run_nvcc.cmake")
endif()

common_find_package(OptiX REQUIRED SYSTEM)

include_directories(${OptiX_INCLUDE_DIRS})

set(BRAYNS_OPTIX_ENGINE_OMIT_LIBRARY_HEADER ON)
set(BRAYNS_OPTIX_ENGINE_OMIT_VERSION_HEADERS ON)
set(BRAYNS_OPTIX_ENGINE_EXPORT ON)

set(BRAYNS_OPTIX_ENGINE_CU
  cuda/camera/PerspectiveCamera.cu
  cuda/Constantbg.cu
  cuda/geometry/Cones.cu
  cuda/geometry/Cylinders.cu
  cuda/geometry/Spheres.cu
  cuda/geometry/TrianglesMesh.cu
  cuda/renderer/AdvancedSimulation.cu
  cuda/renderer/BasicRenderer.cu
)

CUDA_GET_SOURCES_AND_OPTIONS(source_files cmake_options options ${ARGN})
CUDA_WRAP_SRCS(brayns_optix_engine
  PTX generated_files ${BRAYNS_OPTIX_ENGINE_CU} ${cmake_options}
  OPTIONS ${options}
)

set(BRAYNS_OPTIX_ENGINE_PTX
  brayns_optix_engine_generated_PerspectiveCamera.cu.ptx
  brayns_optix_engine_generated_Constantbg.cu.ptx
  brayns_optix_engine_generated_Cones.cu.ptx
  brayns_optix_engine_generated_Cylinders.cu.ptx
  brayns_optix_engine_generated_Spheres.cu.ptx
  brayns_optix_engine_generated_TrianglesMesh.cu.ptx
  brayns_optix_engine_generated_AdvancedSimulation.cu.ptx
  brayns_optix_engine_generated_BasicRenderer.cu.ptx
)
include(StringifyPtx)
stringify_ptx(${BRAYNS_OPTIX_ENGINE_PTX})

set(BRAYNS_OPTIX_ENGINE_SOURCES
  ${PTX_SOURCES}
  OptiXContext.cpp
  OptiXFrameBuffer.cpp
  OptiXScene.cpp
  OptiXCamera.cpp
  OptiXRenderer.cpp
  OptiXEngine.cpp
  OptiXMaterial.cpp
  OptiXModel.cpp
)

set_source_files_properties(
  OptiXContext.cpp
  OptiXFrameBuffer.cpp
  OptiXScene.cpp
  OptiXCamera.cpp
  OptiXRenderer.cpp
  OptiXEngine.cpp
  OptiXMaterial.cpp
  OptiXModel.cpp
  PROPERTIES COMPILE_FLAGS -Wno-shadow
)

set(BRAYNS_OPTIX_ENGINE_PUBLIC_HEADERS
  ${PTX_HEADERS}
  OptiXContext.h
  OptiXFrameBuffer.h
  OptiXScene.h
  OptiXCamera.h
  OptiXRenderer.h
  OptiXEngine.h
)

set(BRAYNS_OPTIX_ENGINE_LINK_LIBRARIES
    PRIVATE braynsEngine vmmlib braynsCommon braynsParameters ${OptiX_LIBRARIES})

common_library(brayns_optix_engine)

if(CUDA_VERSION VERSION_GREATER 8)
  target_compile_options(brayns_optix_engine PRIVATE "-Wno-error=cpp")
endif()
