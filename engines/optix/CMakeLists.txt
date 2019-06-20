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

# Patch for cmake 3.7->3.9
if(CMAKE_VERSION VERSION_GREATER "3.6" AND CMAKE_VERSION VERSION_LESS "3.10")
  set(CUDA_run_nvcc "${CMAKE_CURRENT_SOURCE_DIR}/CMake/run_nvcc.cmake")
endif()

common_find_package(OptiX REQUIRED SYSTEM)

include_directories(${OptiX_INCLUDE_DIRS})

set(BRAYNSOPTIXENGINE_OMIT_LIBRARY_HEADER ON)
set(BRAYNSOPTIXENGINE_OMIT_VERSION_HEADERS ON)
set(BRAYNSOPTIXENGINE_EXPORT ON)

set(BRAYNSOPTIXENGINE_CU
  cuda/camera/PerspectiveCamera.cu
  cuda/Constantbg.cu
  cuda/geometry/Cones.cu
  cuda/geometry/Cylinders.cu
  cuda/geometry/Spheres.cu
  cuda/geometry/TriangleMesh.cu
  cuda/renderer/AdvancedSimulation.cu
  cuda/renderer/BasicSimulation.cu
  cuda/renderer/PBR.cu
)

CUDA_GET_SOURCES_AND_OPTIONS(source_files cmake_options options ${ARGN})
CUDA_WRAP_SRCS(braynsOptixEngine
  PTX generated_files ${BRAYNSOPTIXENGINE_CU} ${cmake_options}
  OPTIONS ${options}
)

set(BRAYNSOPTIXENGINE_PTX
  braynsOptixEngine_generated_PerspectiveCamera.cu.ptx
  braynsOptixEngine_generated_Constantbg.cu.ptx
  braynsOptixEngine_generated_Cones.cu.ptx
  braynsOptixEngine_generated_Cylinders.cu.ptx
  braynsOptixEngine_generated_Spheres.cu.ptx
  braynsOptixEngine_generated_TriangleMesh.cu.ptx
  braynsOptixEngine_generated_AdvancedSimulation.cu.ptx
  braynsOptixEngine_generated_BasicSimulation.cu.ptx
  braynsOptixEngine_generated_PBR.cu.ptx
)

include(StringifyPtx)
stringify_ptx(${BRAYNSOPTIXENGINE_PTX})

set(BRAYNSOPTIXENGINE_SOURCES
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

set(BRAYNSOPTIXENGINE_PUBLIC_HEADERS
  ${PTX_HEADERS}
  OptiXContext.h
  OptiXFrameBuffer.h
  OptiXScene.h
  OptiXCamera.h
  OptiXRenderer.h
  OptiXEngine.h
)

set(BRAYNSOPTIXENGINE_LINK_LIBRARIES
    PRIVATE braynsEngine glm braynsCommon braynsParameters ${OptiX_LIBRARIES})

common_library(braynsOptixEngine)

if(CUDA_VERSION VERSION_GREATER 8)
  target_compile_options(braynsOptixEngine PRIVATE "-Wno-error=cpp")
endif()
