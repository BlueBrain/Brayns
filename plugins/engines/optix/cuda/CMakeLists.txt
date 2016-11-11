# Copyright (c) 2015-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

# Configure the output directories. To allow IMPI to do its magic we
# will put *executables* into the (same) build directory, but tag
# mic-executables with ".mic". *libraries* cannot use the
# ".mic"-suffix trick, so we'll put libraries into separate
# directories (names 'intel64' and 'mic', respectively)

set(BRAYNSOPTIXCUDAPLUGIN_SOURCES
  geometry/Cylinders.cu
  geometry/Cones.cu
  geometry/Spheres.cu
  geometry/TrianglesMesh.cu
  renderer/Phong.cu
  camera/Camera.cu
  Constantbg.cu
)

set(BRAYNSOPTIXCUDAPLUGIN_PUBLIC_HEADERS
  geometry/IntersectionRefinement.h
  renderer/Phong.h
  CommonStructs.h
  Helpers.h
  Random.h
)

# Ensure ptx are in the same relative path for build and install directories
set(_ptx_rel_dir share/${PROJECT_NAME}/ptx)
set(CUDA_GENERATED_OUTPUT_DIR ${CMAKE_BINARY_DIR}/${_ptx_rel_dir})
install(
  FILES
    ${CUDA_GENERATED_OUTPUT_DIR}/braynsOptiXCudaPlugin_generated_Camera.cu.ptx
    ${CUDA_GENERATED_OUTPUT_DIR}/braynsOptiXCudaPlugin_generated_Cones.cu.ptx
    ${CUDA_GENERATED_OUTPUT_DIR}/braynsOptiXCudaPlugin_generated_Spheres.cu.ptx
    ${CUDA_GENERATED_OUTPUT_DIR}/braynsOptiXCudaPlugin_generated_Cylinders.cu.ptx
    ${CUDA_GENERATED_OUTPUT_DIR}/braynsOptiXCudaPlugin_generated_TrianglesMesh.cu.ptx
    ${CUDA_GENERATED_OUTPUT_DIR}/braynsOptiXCudaPlugin_generated_Constantbg.cu.ptx
    ${CUDA_GENERATED_OUTPUT_DIR}/braynsOptiXCudaPlugin_generated_Phong.cu.ptx
  DESTINATION
    ${_ptx_rel_dir}
)

# These calls will group PTX and CUDA files into their own directories in the Visual
# Studio projects.
source_group("PTX Files"  REGULAR_EXPRESSION ".+\\.ptx$")
source_group("CUDA Files" REGULAR_EXPRESSION ".+\\.cu$")

# Separate the sources from the CMake and CUDA options fed to the macro.  This code
# comes from the CUDA_COMPILE_PTX macro found in FindCUDA.cmake.  We are copying the
# code here, so that we can use our own name for the target.  target_name is used in the
# creation of the output file names, and we want this to be unique for each target in
# the SDK.
CUDA_GET_SOURCES_AND_OPTIONS(
  source_files cmake_options options ${ARGN}
)

# Create the rules to build the PTX from the CUDA files.
CUDA_WRAP_SRCS(
  braynsOptiXCudaPlugin PTX generated_files ${BRAYNSOPTIXCUDAPLUGIN_SOURCES} ${cmake_options}
  OPTIONS ${options}
)

set(BRAYNSOPTIXCUDAPLUGIN_LINK_LIBRARIES
  PRIVATE ${OptiX_LIBRARIES}
)
common_library(braynsOptiXCudaPlugin)
