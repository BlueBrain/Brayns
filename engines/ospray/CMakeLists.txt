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

# compiler selection for the non-MIC part
if(APPLE)
  set(OSPRAY_COMPILER "CLANG" CACHE STRING "compiler to use (ICC,GCC, or CLANG)")
  set_property(CACHE OSPRAY_COMPILER PROPERTY STRINGS ICC GCC CLANG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-variable-sized-type-not-at-end")
else()
  set(OSPRAY_COMPILER "GCC" CACHE STRING "compiler to use (ICC or GCC)")
  set_property(CACHE OSPRAY_COMPILER PROPERTY STRINGS ICC GCC)
endif()


set(BRAYNSOSPRAYENGINE_ISPC_SOURCES
  ispc/camera/utils.ispc
  ispc/camera/OrthographicCamera.ispc
  ispc/camera/PanoramicCamera.ispc
  ispc/camera/PerspectiveCamera.ispc
  ispc/camera/FishEyeCamera.ispc
  ispc/camera/PerspectiveParallaxCamera.ispc
  ispc/geometry/Cones.ispc
  ispc/geometry/SDFBeziers.ispc
  ispc/geometry/SDFGeometries.ispc
  ispc/geometry/RayMarching.isph
  ispc/render/BasicRenderer.ispc
  ispc/render/DefaultMaterial.ispc
  ispc/render/utils/RandomGenerator.ispc
  ispc/render/utils/SkyBox.ispc
)

set(BRAYNSOSPRAYENGINE_SOURCES
  OSPRayCamera.cpp
  OSPRayEngine.cpp
  OSPRayFrameBuffer.cpp
  OSPRayMaterial.cpp
  OSPRayModel.cpp
  OSPRayRenderer.cpp
  OSPRayScene.cpp
  OSPRayVolume.cpp
  utils.cpp
  ispc/camera/OrthographicCamera.cpp
  ispc/camera/PanoramicCamera.cpp
  ispc/camera/FishEyeCamera.cpp
  ispc/camera/PerspectiveCamera.cpp
  ispc/camera/PerspectiveParallaxCamera.cpp
  ispc/geometry/Cones.cpp
  ispc/geometry/SDFBeziers.cpp
  ispc/geometry/SDFGeometries.cpp
  ispc/render/BasicRenderer.cpp
  ispc/render/DefaultMaterial.cpp
  ispc/render/utils/AbstractRenderer.cpp
)
list(APPEND BRAYNSOSPRAYENGINE_SOURCES ${BRAYNSOSPRAYENGINE_ISPC_SOURCES})

set(BRAYNSOSPRAYENGINE_HEADERS
  utils.h
  OSPRayCamera.h
  OSPRayEngine.h
  OSPRayFrameBuffer.h
  OSPRayMaterial.h
  OSPRayModel.h
  OSPRayRenderer.h
  OSPRayScene.h
  OSPRayVolume.h
  ispc/camera/OrthographicCamera.h
  ispc/camera/PanoramicCamera.h 
  ispc/camera/PerspectiveCamera.h
  ispc/camera/FishEyeCamera.h
  ispc/camera/PerspectiveParallaxCamera.h
  ispc/render/BasicRenderer.h
  ispc/render/DefaultMaterial.h
  ispc/render/utils/AbstractRenderer.h
)

set(BRAYNSOSPRAYENGINE_PUBLIC_HEADERS
  ispc/geometry/Cones.h
  ispc/geometry/SDFBeziers.h
  ispc/geometry/SDFGeometries.h
)

set_source_files_properties(
    OSPRayScene.cpp
    OSPRayMaterial.cpp
    PROPERTIES COMPILE_FLAGS -Wno-deprecated-declarations)

set(BRAYNSOSPRAYENGINE_LINK_LIBRARIES
    PRIVATE braynsEngine braynsParameters braynsCommon ospray::ospray_common ospray::ospray
    # Public linkage needed for transfer function test
    PUBLIC ospray::ospray_module_ispc
)

# reuse ispc setup and macros from ospray
list(APPEND CMAKE_MODULE_PATH ${OSPRAY_CMAKE_ROOT})
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(OSPRAY_DEBUG_BUILD ON)
endif()
include(osprayUse)

# Enable/disable use of hardware randomizer
option(BRAYNS_HARDWARE_RANDOMIZER_ENABLED "Enables hardware randomizer if supported by target CPU" OFF)
if(BRAYNS_HARDWARE_RANDOMIZER_ENABLED)
  set(ISPC_EXTRA_ARGS -DBRAYNS_ISPC_USE_HARDWARE_RANDOMIZER=ON)
endif()

# Compile ispc code
include_directories_ispc(${PROJECT_SOURCE_DIR})
ospray_ispc_compile(${BRAYNSOSPRAYENGINE_ISPC_SOURCES})
list(APPEND BRAYNSOSPRAYENGINE_SOURCES ${ISPC_OBJECTS})

# Compile C++ code
set(BRAYNSOSPRAYENGINE_OMIT_LIBRARY_HEADER ON)
set(BRAYNSOSPRAYENGINE_OMIT_VERSION_HEADERS ON)
set(BRAYNSOSPRAYENGINE_EXPORT ON)

common_library(braynsOSPRayEngine)
