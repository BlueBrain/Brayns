# Copyright (c) 2015-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

# For FindOptix and CUDA patch
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

common_find_package(CUDA REQUIRED SYSTEM)
if(CUDA_FOUND)
  # This doesn't get called from common_find_package_post unless we export some
  # variables to the parent scope
  find_cuda_compatible_host_compiler()
endif()
message(STATUS "Using CUDA version ${CUDA_VERSION}")

set(CUDA_NVCC_FLAGS "--use_fast_math")

# Patch for cmake 3.7->3.9
if(CMAKE_VERSION VERSION_GREATER "3.6" AND CMAKE_VERSION VERSION_LESS "3.10")
  set(CUDA_run_nvcc "${CMAKE_CURRENT_SOURCE_DIR}/CMake/run_nvcc.cmake")
endif()

common_find_package(OptiX REQUIRED SYSTEM)

set(BRAYNSOPTIXENGINE_OMIT_LIBRARY_HEADER ON)
set(BRAYNSOPTIXENGINE_OMIT_VERSION_HEADERS ON)

set(BRAYNSOPTIXENGINE_CU
  cuda/camera/PerspectiveCamera.cu
  cuda/camera/OpenDeckCamera.cu
  cuda/Constantbg.cu
  cuda/geometry/Cones.cu
  cuda/geometry/Cylinders.cu
  cuda/geometry/Spheres.cu
  cuda/geometry/TriangleMesh.cu
  cuda/renderer/AdvancedSimulation.cu
  cuda/renderer/BasicSimulation.cu
  cuda/renderer/PBR.cu
  cuda/renderer/BBP.cu
)

CUDA_WRAP_SRCS(braynsOptixEngine PTX ptx_generated_files ${BRAYNSOPTIXENGINE_CU})

include(StringifyPtx)
stringify_ptx(${ptx_generated_files})

set(BRAYNSOPTIXENGINE_SOURCES
  ${BRAYNSOPTIXENGINE_CU}
  ${ptx_generated_files}
  ${PTX_SOURCES}
  OptiXContext.cpp
  OptiXFrameBuffer.cpp
  OptiXScene.cpp
  OptiXCamera.cpp
  OptiXPerspectiveCamera.cpp
  OptiXOpenDeckCamera.cpp
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
  OptiXPerspectiveCamera.cpp
  OptiXOpenDeckCamera.cpp
  OptiXRenderer.cpp
  OptiXEngine.cpp
  OptiXMaterial.cpp
  OptiXModel.cpp
  PROPERTIES COMPILE_FLAGS -Wno-shadow
)

set(BRAYNSOPTIXENGINE_HEADERS
  ${PTX_HEADERS}
  OptiXContext.h
  OptiXFrameBuffer.h
  OptiXScene.h
  OptiXCamera.h
  OptiXCameraProgram.h
  OptiXOpenDeckCamera.h
  OptiXPerspectiveCamera.h
  OptiXRenderer.h
  OptiXEngine.h
)

set(BRAYNSOPTIXENGINE_LINK_LIBRARIES
    PRIVATE braynsEngine glm braynsCommon braynsParameters ${OptiX_LIBRARIES})

common_library(braynsOptixEngine)

if(CUDA_VERSION VERSION_GREATER 8)
  target_compile_options(braynsOptixEngine PRIVATE "-Wno-error=cpp")
endif()
